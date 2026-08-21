@echo off
echo t.me/nez3rpw
setlocal EnableExtensions
pushd "%~dp0"
set "NDK=C:\android-ndk-r27d"
if not exist "build-cmake-arm64" mkdir "build-cmake"
cmake -S . -B build-cmake-arm64 -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE="%NDK%\build\cmake\android.toolchain.cmake" ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-23 ^
    -DANDROID_STL=c++_static ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build-cmake-arm64 -j%NUMBER_OF_PROCESSORS%
popd

echo cmake exit with code %errorlevel%
