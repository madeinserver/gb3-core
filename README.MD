# GameBryo3 Core
This is a forked, modularized and cross-platform version of GameBryo 3.2.0.661.

## Motivation
The engine is very flexibe by design, but the build system was very static and modules
(such as DX9 Renderer) were imported into various core component.

The idea of this branch is to avoid importing platform-specific functionalities by default and replace them with cross-platform ones that, with also avoiding direct importing of third-party or modules that specifically require platform-specific code to operate.

## What's included
This module contains the main code of the engine, everything that you need to setup an GameBryo core environment under Windows or Linux.

External integrations and similar software (or a custom backend) has to be manually places in the Integration directory in order to be included. (by default, it does not come with any renderer yet)

## Supported platform
- Windows (trough SDL2 or Win32 backend)
- Linux (trough SDL2, needs testing)
- MacOSX (trough SDL2, needs testing)

## Supported compilers
- Visual Studio (MSVC)
- GCC
- CLang (needs testing)

## Required dependencies
- vcpkg
- NASM
- CMake
- A C++ compiler
