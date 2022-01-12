#!/bin/bash

./boost-url-generate.sh

root=$(cd ../.. ; pwd)
cd ${root}
profile=${root}/tools/conan/profiles/gcc-linux

# Build beasty tepee
rm -rf build
mkdir build
cd build
conan install .. --build missing -s build_type=Debug --profile=${profile}
cmake .. -DCMAKE_BUILD_TYPE=Debug
