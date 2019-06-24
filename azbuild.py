#!/usr/bin/env python3

"""
Much of this code is taken from the xenia build script
https://github.com/xenia-project/xenia/blob/master/xenia-build
"""
import argparse
import configparser
import os
import subprocess
import sys
import json

__author__ = "Satori (Razzile) <https://github.com/Razzile>"

config = configparser.ConfigParser()


def discover_commands(subparsers):
    """Looks for all commands and returns a dictionary of them.
    In the future commands could be discovered on disk.
    Args:
      subparsers: Argument subparsers parent used to add command parsers.
    Returns:
      A dictionary containing name-to-Command mappings.
    """
    commands = {
        'setup': SetupCommand(subparsers),
    }
    # if sys.platform == 'win32':
    #    commands['devenv'] = DevenvCommand(subparsers)
    return commands


def main():
    try:
        with open("config.ini") as f:
            config.read_file(f)
    except Exception as e:
        print(
            "No config file found. please make sure there is a valid config.ini at %s\nError: %s"
            % (os.getcwd(), str(e)))
        sys.exit(1)

    # Setup main argument parser and common arguments.
    parser = argparse.ArgumentParser(prog='azbuild.py')

    # Grab all commands and populate the argument parser for each.
    subparsers = parser.add_subparsers(title='subcommands',
                                       dest='subcommand')
    commands = discover_commands(subparsers)

    # If the user passed no args, die nicely.
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)

    # Gather any arguments that we want to pass to child processes.
    command_args = sys.argv[1:]
    pass_args = []
    try:
        pass_index = command_args.index('--')
        pass_args = command_args[pass_index + 1:]
        command_args = command_args[:pass_index]
    except Exception:
        pass

    # Parse command name and dispatch.
    args = vars(parser.parse_args(command_args))
    command_name = args['subcommand']
    try:
        command = commands[command_name]
        return_code = command.execute(args, pass_args, os.getcwd())
    except Exception:
        raise
    sys.exit(return_code)


def shell_call(command, throw_on_error=True, stdout_path=None):
    """Executes a shell command.
    Args:
      command: Command to execute, as a list of parameters.
      throw_on_error: Whether to throw an error or return the status code.
      stdout_path: File path to write stdout output to.
    Returns:
      If throw_on_error is False the status code of the call will be returned.
    """
    stdout_file = None
    if stdout_path:
        stdout_file = open(stdout_path, 'w')
    result = 0
    try:
        if throw_on_error:
            result = 1
            subprocess.check_call(command, shell=False, stdout=stdout_file)
            result = 0
        else:
            result = subprocess.call(command, shell=False, stdout=stdout_file)
    finally:
        if stdout_file:
            stdout_file.close()
    return result


def git_submodule_update():
    shell_call([
        'git',
        'submodule',
        'update',
        '--init',
        '--recursive',
    ])

# cmake "-GAndroid Gradle - Ninja" -DANDROID_ABI=arm64-v8a -DANDROID_NDK="D:\Programs\Android\SDK\ndk-bundle"
# -DCMAKE_MAKE_PROGRAM=ninja -DCMAKE_TOOLCHAIN_FILE="D:\Programs\Android\SDK\ndk-bundle\build\cmake\android.toolchain.cmake"
# -DANDROID_NATIVE_API_LEVEL=21 -DANDROID_TOOLCHAIN=clang


def build(target, args):
    if target == "android":

        if config.has_option("Android", "sdk_location"):
            raise Exception("No Android SDK location provided")

        sdk_loc = config.get("Android", "sdk_location")
        arch = args.arch if args.arch else "arm64-v8a"
        abi_level = args.api_level if args.api_level else "21"

        flags = """
        -DANDROID_ABI={0} -DANDROID_NDK=\"{1}/nkd-bundle\"
        -DCMAKE_MAKE_PROGRAM=ninja -DCMAKE_TOOLCHAIN_FILE=\"{1}/nkd-bundle/build/cmake/android.toolchain.cmake\" 
        -DANDROID_NATIVE_API_LEVEL={2} -DANDROID_TOOLCHAIN=clang""".format(arch, sdk_loc, abi_level)


class Command(object):
    """Base type for commands.
    """

    def __init__(self, subparsers, name, help_short=None, help_long=None,
                 *args, **kwargs):
        """Initializes a command.
        Args:
          subparsers: Argument subparsers parent used to add command parsers.
          name: The name of the command exposed to the management script.
          help_short: Help text printed alongside the command when queried.
          help_long: Extended help text when viewing command help.
        """
        self.name = name
        self.help_short = help_short
        self.help_long = help_long

        self.parser = subparsers.add_parser(name,
                                            help=help_short,
                                            description=help_long)
        self.parser.set_defaults(command_handler=self)

    def execute(self, args, pass_args, cwd):
        """Executes the command.
        Args:
          args: Arguments hash for the command.
          pass_args: Arguments list to pass to child commands.
          cwd: Current working directory.
        Returns:
          Return code of the command.
        """
        return 1


class SetupCommand(Command):
    """'setup' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(SetupCommand, self).__init__(
            subparsers,
            name='setup',
            help_short='Setup the build environment.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Setting up the build environment...')
        print('')

        # Setup submodules.
        print('- git submodule init / update...')
        git_submodule_update()
        print('')

        return 0


if __name__ == '__main__':
    main()
