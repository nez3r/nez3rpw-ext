@echo off
set "bin=libs\arm64-v8a\nez3r.sh"
adb shell su -c killall -9 nez3r.sh
adb push %bin% /data/local/tmp/nez3r.sh
adb shell "su -c 'chmod 777 /data/local/tmp/nez3r.sh'"
adb shell "su -c /data/local/tmp/nez3r.sh"
echo.
