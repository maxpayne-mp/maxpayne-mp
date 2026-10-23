@echo off
setlocal
set "ROOT=%~dp0"
set "TARGET=%~1"
set "CONFIG=%~2"
if "%CONFIG%"=="" set "CONFIG=Release"
if "%TARGET%"=="" set "TARGET=all"

if /I "%TARGET%"=="all" (
    set "SRC=%ROOT%."
    set "BUILD=%ROOT%build\all"
    set "TARGET_ARG="
) else if /I "%TARGET%"=="mp" (
    set "SRC=%ROOT%maxpayne-mp"
    set "BUILD=%ROOT%build\mp"
    set "TARGET_ARG=--target maxpayne-mp"
) else if /I "%TARGET%"=="server" (
    set "SRC=%ROOT%maxpayne-server"
    set "BUILD=%ROOT%build\server"
    set "TARGET_ARG=--target maxpayne-server"
) else if /I "%TARGET%"=="launcher" (
    set "SRC=%ROOT%maxpayne-injector"
    set "BUILD=%ROOT%build\launcher"
    set "TARGET_ARG=--target maxpayne-launcher"
) else (
    echo Uso: build.bat [all^|mp^|server^|launcher] [Release^|Debug]
    exit /b 1
)

cmake -S "%SRC%" -B "%BUILD%" -A Win32
if errorlevel 1 exit /b 1

cmake --build "%BUILD%" --config %CONFIG% %TARGET_ARG% --parallel
if errorlevel 1 exit /b 1

endlocal
