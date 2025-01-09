# JSON CLI Tool

## Overview

This project is a command-line interface (CLI) tool for performing operations on JSON files. It is built using C++ and leverages the Boost libraries for handling program options, filesystem operations, and JSON parsing.

## Features

- Print the contents of a JSON file in a pretty format
- Retrieve and print specific values from a JSON file
- Cross-platform support (Linux, macOS, and Windows)

## Requirements

- CMake 3.30 or higher
- C++17 compatible compiler
- Boost libraries (program_options, filesystem, json)

## Building the Project

1. Create a build directory and navigate to it:
    ```sh
    mkdir build
    cd build
    ```

2. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

3. Build the project:
    ```sh
    make
    ```

## Installing the Executable

To install the executable to the default location (`/usr/local/bin` on Linux/macOS or `C:\Program Files\json` on Windows):

```sh
sudo cmake --install .
```

For windows, you can also use the following command:

```sh
cmake -DCMAKE_INSTALL_PREFIX="C:\Program Files\json" ..
cmake --build . --target install --config Release
```