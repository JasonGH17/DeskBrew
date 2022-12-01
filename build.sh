#!/bin/bash

set -e

mkdir -p bin

pushd src
echo Building project...

pushd Engine
/bin/bash ./build.sh
popd

# pushd Shader
# /bin/bash ./linux_build.bat
# popd

pushd App
/bin/bash ./build.sh
popd

echo Successfully built the project!