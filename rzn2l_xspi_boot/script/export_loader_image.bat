@echo off
setlocal

if "%~1"=="" (
    echo ERROR: Application ELF path is required.
    exit /b 1
)

set "OUTPUT_DIR=%~dp0..\build\loader_input"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

rem Export only initialized App image contents.  The excluded sections have
rem distant runtime addresses (SDRAM/HyperRAM and BSS); a raw binary would
rem otherwise fill the address gaps and exceed xSPI0 CS0.
arm-none-eabi-objcopy -O binary ^
    --remove-section=.sdram_nc ^
    --remove-section=.heap ^
    --remove-section=.bss ^
    --remove-section=.thread_stack ^
    --remove-section=.dmac_link_mode ^
    --remove-section=.shared_noncache_buffer ^
    --remove-section=.noncache_buffer ^
    "%~1" "%OUTPUT_DIR%\application.bin"
if errorlevel 1 exit /b %errorlevel%

echo Loader input image updated: %OUTPUT_DIR%\application.bin
