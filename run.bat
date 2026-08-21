@echo off
set "bin=build-cmake-arm64\nez3r"
adb shell su -c killall -9 nez3r
adb push %bin% /data/local/tmp/nez3r
adb shell "su -c 'chmod 777 /data/local/tmp/nez3r'"
adb shell "su -c /data/local/tmp/nez3r"
echo.
