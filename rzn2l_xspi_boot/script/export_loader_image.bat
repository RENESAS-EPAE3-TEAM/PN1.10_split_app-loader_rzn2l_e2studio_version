@echo off
setlocal

if "%~1"=="" (
    echo ERROR: Application ELF path is required.
    exit /b 1
)

set "OUTPUT_DIR=%~dp0..\build\loader_input"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

arm-none-eabi-objcopy -O binary "%~1" "%OUTPUT_DIR%\application.bin"
if errorlevel 1 exit /b %errorlevel%

echo Loader input image updated: %OUTPUT_DIR%\application.bin
