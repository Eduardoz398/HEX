@echo off
cls

set "root=code"

g++ -o "%root%\main.exe" "%root%\main.cpp" -std=c++17

if %errorlevel% == 0 (
    "%root%\main.exe"
) else (
    echo Erro na compilação
)

pause
