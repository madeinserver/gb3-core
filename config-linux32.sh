#!/bin/sh
mkdir -p Build/Linux_x86_32bit/
cd Build/Linux_x86_32bit/
cmake -G 'Ninja Multi-Config' -DCMAKE_TOOLCHAIN_FILE="../../Source/CMake/Linux32.toolchain" ../../
