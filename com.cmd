@echo off
chcp 65001 > nul
set PATH=%PATH%C:\Program Files\Git\bin\;
if not exist .git (
    echo В этой папке нет Git-репозитория!
    echo Запустите сначала команду 'git init' или свяжите папку с GitHub.
    exit /b
)
echo.
git add .
set /p commit_message="Введите сообщение для коммита: "
if "%commit_message%"=="" (
    echo В коммите пусто.
    exit /b
)
git commit -m "%commit_message%"
echo.
echo Отправка кода на GitHub...
git push origin main
