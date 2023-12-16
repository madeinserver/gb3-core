# Changes in the Made In Server forked version

## Build system
The whole Makefile/Visual Studio projects have been removed and changed with CMake along with custom support for different integrations.

vcpkg was also added to simplify the managing of third party libraries.

NASM is required as the cross-platform standalone assembler for x86 (both 32 and 64 bits) platform.

GameBryo can be linked in a game with two ways, assuming it supports CMake:
1. The system will try to find a system-wide installation of the engine, it can be easily invoked by adding the path to the FindGameBryo.cmake setted up via CMake install function
2. Manually adding the GameBryo to your game sources by using the FindGameBryoSource.cmake module

## Backend changes
The new universal backend SDL2 is now implemented, the core code no longer depends on DirectX SDK
and everythihg has been abstracted to avoid/decoupled to avoid dependencies on such SDK.

Please note that the code still doesn't support other platforms provided by SDL2, such as Android or iOS as that would require many changes for Touch support.

Linux backend was not provided as part of any leaked code, therefore I was not able to use and fix that.

### Special backend notes
- Mouse X3-X4-X5 buttons no longer works due to limitations in SDL2 API, any code that checks for those will not work anymore.
- X axis on Mouse wheel is not tracked by Gamebryo, it might require special changes later to add this support.
- joystick input pooling can no longer be disabled

## New supports
- Added support for Linux in the core package
- Added support for MacOSX in the code package
- Added support for CLang
- Fixed warning/errors relative to linux and gcc builds
- 64-bit code is work in progress

More test is needed to determine the correct workness of such platforms and compilers.

## Dynamic libraries
- Currently, no support is being provided for Dynamic libraries, altrough they are required for a correct functionality of MaxImmerse or MayaImmerse

## PS3/Xbox360/Wii support
- Wii support was removed as of GameBryo 3.0, if someone wants to restore it they could do that by checking on the sources of GameBryo 2.6 and begin to merge the relative changes in the core.
Keep in mind that all efd* egm* packages would need support as well and that the Wii lacked several modules by default.
- PS3/Xbox360 support still exists, but due to the unability to have an open-source backend that works with the homebrew SDKs, no support will be provided.

## API Changes
- References counting and atomic use now requires to use the special "efd::SAtomic" and "efd::UAtomic" types, this is because some platforms (like Linux) uses 64-bit atomic types only.
- NiMilesAudio is no longer provided in the basic core, due to it's linkness with a closed source API.
- NiBinkVideo is no longer provided in the basic code, for the same reason as Miles.
- The core package does **not** include any renderer, due to the unability to find/have a global render api that can work on all platforms.
- Added efd::WindowNativeRef and NiWindowNativeRef for native (HWND on Windows) reference
