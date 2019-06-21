# Azure Server
This repo contains the source for a reference implementation of the server for Azure. This server currently supports Android and Windows, with support for iOS, macOS and Linux planned eventually. 

⚠This code is still a Work-In-Progress⚠

## Building

To buil this azure server for your desired platform, you can use the integrated build script `azbuild`.

1. clone this repo
2. run `azbuild setup` to setup dependencies 
3. copy the `config.example.ini` file to `config.ini` and modify the values inside the config to your requirements
4. run `azbuild build --target={android,windows}` to build for that target. Omitting the target will build for the current host system (if supported).
5. `azbuild --help` shows the full lists of commands supported

This build script is still a work-in-progress. It is possible to use cmake directly to build this project but this isn't recommended as there are a large amount of flags that must be passed to cmake for a successful compilation.