where /q cmake
if ERRORLEVEL 1 (
  set "PATH=C:\Program Files\CMake\bin;%PATH%"
)
pushd %~dp0..\..
set root=%CD%
set "profile=%root%/tools/conan/profiles/msvc"

Rem Build boost.url
pushd tools\conan\url
conan create . boost.url/20200728@ --build missing -s build_type=Debug --profile=%profile%
popd

Rem Build beasty tepee
rmdir /Q /S build
mkdir build
cd build
conan install .. --build missing -s build_type=Debug --profile=%profile%
cmake ..
popd
