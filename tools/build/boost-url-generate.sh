#!/bin/bash

root=$(cd ../.. ; pwd)
cd ${root}
profile=${root}/tools/conan/profiles/gcc-linux

# Build boost.url
pushd tools/conan/url
conan create . boost.url/20200521@ --build missing -s build_type=Debug --profile=${profile}
popd

conan install . --build missing -s build_type=Debug --profile=${profile}