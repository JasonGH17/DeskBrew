#!/bin/bash

set -e

files=$(find . -type f -name '*.cpp')

outName="DeskBrew"
cstd="-std=c++17"
cflags="-ggdb -Wall -Werror -Wextra -pedantic"
include="-I../ -I../Engine"
linker="-L../../bin/ -Wl,-rpath,. -lengine"
defines="-D_DEBUG -DDB_IMPORT"

echo Building $outName executable
g++ -o ../../bin/$outName $files $cstd $include $linker $defines