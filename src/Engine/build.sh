#!/bin/bash

set -e

files=$(find . -type f -name '*.cpp')

outName="engine"
cstd="-std=c++17"
cflags="-ggdb -shared -Wall -Werror -Wextra -pedantic -fPIC"
include="-I.. -I. -I$VULKAN_SDK/include"
linker="-lX11 -lxcb -lX11-xcb -L$VULKAN_SDK/lib -lvulkan"
defines="-D_DEBUG -DVK_DEBUG -DVK_USE_PLATFORM_XCB_KHR -DDB_EXPORT -D_CRT_SECURE_NO_WARNINGS"

echo Building "$outName".so
g++ -o ../../bin/lib"$outName".so $files $cflags $cstd $include $linker $defines