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
import shutil
import json
from os import path

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
        'build': BuildCommand(subparsers),
        'clean': CleanCommand(subparsers)
    }
    if sys.platform == 'win32':
        commands['devenv'] = DevenvCommand(subparsers)
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


def shell_call(command, throw_on_error=True, stdout_path=None, cwd=None):
    """Executes a shell command.
    Args:
      command: Command to execute, as a list of parameters.
      throw_on_error: Whether to throw an error or return the status code.
      stdout_path: File path to write stdout output to.
      cwd: Path to execute the command from
    Returns:
      If throw_on_error is False the status code of the call will be returned.
    """
    stdout_file = None
    if stdout_path:
        stdout_file = open(stdout_path, 'w')

    if cwd is None:
        cwd = os.getcwd()
    result = 0
    try:
        if throw_on_error:
            result = 1
            subprocess.check_call(command, shell=False,
                                  stdout=stdout_file, cwd=cwd)
            result = 0
        else:
            result = subprocess.call(
                command, shell=False, stdout=stdout_file, cwd=cwd)
    finally:
        if stdout_file:
            stdout_file.close()
    return result


def get_bin(binary):
    # from xenia-build script
    """Checks whether the given binary is present and returns the path.
    Args:
      binary: binary name (without .exe, etc).
    Returns:
      Full path to the binary or None if not found.
    """
    for path in os.environ['PATH'].split(os.pathsep):
        path = path.strip('"')
        exe_file = os.path.join(path, binary)
        if os.path.isfile(exe_file) and os.access(exe_file, os.X_OK):
            return exe_file
        exe_file += '.exe'
        if os.path.isfile(exe_file) and os.access(exe_file, os.X_OK):
            return exe_file
    return None


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


def generate_cmake(platform, args, execute=True):
    flags = ""
    if not config.has_option("proto", "protobuf_exe") or not config.has_option("proto", "grpc_cpp_plugin_exe"):
        raise Exception("No protobuf or grpc plugin executable provided")

    build_type = args['config'] if args is not None else "Debug"

    if args['generator'] == 'ninja':
        flags += "-GNinja "
    elif args['generator'] == 'default' and args['platform'] == 'android':
        # on android ninja is the only supported build generator
        # TODO: detect if the cmake being used supports -G"Android Gradle - Ninja"
        flags += "-GNinja "

    flags += "-DPROTOC_EXECUTABLE={0} -DGRPC_CPP_PLUGIN={1} ".format(config.get("proto", "protobuf_exe").strip('"'),
                                                                     config.get("proto", "grpc_cpp_plugin_exe").strip('"'))

    # only append build type if we plan to generate
    if execute:
        flags += "-DCMAKE_BUILD_TYPE={0} ".format(build_type)

    # We don't need to build codegen binaries as we ask the user to provide them
    flags += "-Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_PROTOC_BINARIES=OFF -DgRPC_BUILD_TESTS=OFF -DgRPC_BUILD_CODEGEN=OFF "
    if config.has_option("proto", "use_proto_lite") and config.getboolean("proto", "use_proto_lite"):
        flags += "-DgRPC_USE_PROTO_LITE=on "

    # android-specific build options
    if platform == "android":
        if not config.has_option("android", "sdk_location"):
            raise Exception("No Android SDK location provided")

        sdk_loc = config.get("android", "sdk_location").strip('"')
        ndk_bundle = path.join(sdk_loc, "ndk-bundle")
        toolchain_file = path.join(
            ndk_bundle, "build/cmake/android.toolchain.cmake")

        arch = args['arch'] if args['arch'] is not None else "arm64-v8a"
        abi_level = args['abi_level'] if args['abi_level'] is not None else "21"

        # android configuration
        flags += "-DAZURE_TARGET_PLATFORM=android -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_ARCH_ABI={0} -DANDROID_ABI={0} -DCMAKE_ANDROID_NDK={1} -DANDROID_NDK={1} -DCMAKE_MAKE_PROGRAM=ninja -DCMAKE_TOOLCHAIN_FILE={2} -DANDROID_NATIVE_API_LEVEL={3} -DANDROID_TOOLCHAIN=clang CMAKE_ANDROID_STL_TYPE=c++_static ".format(
            arch, ndk_bundle, toolchain_file, abi_level)

        # grpc run configuration
        flags += "-DRUN_HAVE_POSIX_REGEX=0 -DRUN_HAVE_STD_REGEX=0 -DRUN_HAVE_STEADY_CLOCK=0 "

    if platform == "windows":
        flags += "-DAZURE_TARGET_PLATFORM=windows "

    build_path = path.join(os.getcwd(), "build")
    if not path.exists(build_path):
        os.makedirs(build_path)

    print("cmake .. {0}".format(flags))

    gen_cmd = ["cmake"] + flags.split(" ") + [path.join(build_path, "..")]
    if execute:
        shell_call(gen_cmd, cwd=build_path)
    return flags


def build(platform, args, throw_on_error=True):
    build_path = path.join(os.getcwd(), "build")
    if not path.exists(build_path):
        if throw_on_error:
            raise Exception("Build directory has not been generated")
        else:
            return False

    build_cmd = ["cmake", "--build", "."]

    parallel = args is not None and 'j' in args
    if parallel:
        threads = args['j']
        # TODO: check for cmake support (requires version >= 3.12)
        build_cmd += ['-j', str(threads)]

    targets = args['target']
    if targets:
        build_cmd += ['--target'] + targets

    shell_call(build_cmd, cwd=build_path)


def clean():
    build_path = path.join(os.getcwd(), 'build')
    if path.exists(build_path):
        shutil.rmtree(build_path)

    cmake_settings = build_path = path.join(os.getcwd(), 'CMakeSettings.json')
    if path.exists(cmake_settings) and path.isfile(cmake_settings):
        os.remove(cmake_settings)


def devenv(args):
    if get_bin('devenv') is None:
        raise Exception('devenv.exe is not on PATH')

    cmake_settings_path = path.join(os.getcwd(), 'CMakeSettings.json')
    if not path.exists(cmake_settings_path):
        print('')
        print('Generating Visual Studio CMake Settings...')
        print('')
        generate_cmake_settings(args, cmake_settings_path)
    shell_call(['devenv', os.getcwd()])


def generate_cmake_settings(args, file):
    build_dir = path.join(os.getcwd(), 'build')
    setting = {
        "configurations": [
            {
                "cacheRoot": build_dir
            },
            {
                "name": "Windows-Debug",
                "generator": "Ninja",
                "configurationType": "Debug",
                "buildRoot": build_dir+"\\${name}",
                "installRoot": build_dir+"\\install\\${name}",
                "cmakeCommandArgs": generate_cmake('windows', args, False),
                "buildCommandArgs": "-v",
                "ctestCommandArgs": "",
                "inheritEnvironments": ["msvc_x64_x64"],
                "variables": []
            },
            {
                "name": "Windows-Release",
                "generator": "Ninja",
                "configurationType": "Release",
                "buildRoot": build_dir+"\\${name}",
                "installRoot": build_dir+"\\install\\${name}",
                "cmakeCommandArgs": generate_cmake('windows', args, False),
                "buildCommandArgs": "-v",
                "ctestCommandArgs": "",
                "inheritEnvironments": ["msvc_x64_x64"],
                "variables": []
            },
            {
                "name": "Android-Debug",
                "generator": "Ninja",
                "configurationType": "Debug",
                "buildRoot": build_dir+"\\${name}",
                "installRoot": build_dir+"\\install\\${name}",
                "cmakeCommandArgs": generate_cmake('android', args, False),
                "buildCommandArgs": "-v",
                "ctestCommandArgs": "",
                "inheritEnvironments": ["gcc-arm"],
                "intelliSenseMode": "linux-gcc-arm"
            },
            {
                "name": "Android-Release",
                "generator": "Ninja",
                "configurationType": "Release",
                "buildRoot": build_dir+"\\${name}",
                "installRoot": build_dir+"\\install\\${name}",
                "cmakeCommandArgs": generate_cmake('android', args, False),
                "buildCommandArgs": "-v",
                "ctestCommandArgs": "",
                "inheritEnvironments": ["gcc-arm"],
                "intelliSenseMode": "linux-gcc-arm"
            }
        ]
    }

    json_code = json.dumps(setting)

    settings_file = open(file, 'w+')
    settings_file.write(json_code)
    settings_file.close()


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
        print('[Setting up the build environment...]')
        print('')

        # Setup submodules.
        print('executing: git submodule init / update...')
        git_submodule_update()
        print('')

        return 0


class BaseBuildCommand(Command):
    """'build' command"""

    def __init__(self, subparsers, *args, **kwargs):
        super(BaseBuildCommand, self).__init__(
            subparsers,
            *args, **kwargs)
        self.parser.add_argument('--generator', choices=[
                                 'default', 'ninja'], default='default', help='Select the default build generator')
        self.parser.add_argument(
            '--config', choices=['debug', 'release'], default='debug',
            type=str.lower, help='Chooses the build configuration.')
        self.parser.add_argument(
            '--platform', default='windows',
            help='Builds only the given platform(s).')
        self.parser.add_argument(
            '--target', action='append', default=[], help='Builds the specific target(s)')
        self.parser.add_argument(
            '--arch', help='Choose a specific arch to build for')
        self.parser.add_argument(
            '--abi_level', help='Choose a specific android ABI level to build for', default='21')
        self.parser.add_argument(
            '-j', default=4, type=int, help='Number of parallel threads')


class BuildCommand(BaseBuildCommand):
    """'build' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(BuildCommand, self).__init__(
            subparsers,
            name='build',
            help_short='Build the project',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('[Building azure...]')
        print('')

        # Setup submodules.
        print('[Generating cmake project...]')
        print('')
        generate_cmake(args['platform'], args)
        print('[Building cmake project...]')
        print('')
        build(args['platform'], args)
        print('')

        return 0


class CleanCommand(Command):
    """'clean' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(CleanCommand, self).__init__(
            subparsers,
            name='clean',
            help_short='Clean the build environment.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('[Cleaning the build environment...]')
        print('')

        clean()
        print('')

        return 0


class DevenvCommand(BaseBuildCommand):
    """'devenv' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(DevenvCommand, self).__init__(
            subparsers,
            name='devenv',
            help_short='Open the project in visual studio',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        try:
            print('')
            print('WARNING: This command is still experimental')
            print('')
            devenv(args)
        except Exception as e:
            print(str(e))

        return 0


if __name__ == '__main__':
    main()
