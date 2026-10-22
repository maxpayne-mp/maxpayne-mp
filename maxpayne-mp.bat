@echo off
setlocal
set "ROOT=%~dp0"
set "DESTINO=C:\Users\pituxo\Downloads\Max-Payne-SteamRIP.com\Max Payne\maxpayne-mp"
set "DLL=%ROOT%maxpayne-mp\Release\maxpayne-mp.dll"
set "LAUNCHER=%ROOT%maxpayne-injector\Release\maxpayne-launcher.exe"
set "SERVER=%ROOT%maxpayne-server\Release\maxpayne-server.exe"

if not exist "%DESTINO%" mkdir "%DESTINO%"
if not exist "%DESTINO%" (
    echo ERRO: nao foi possivel criar a pasta "%DESTINO%".
    exit /b 1
)

echo Copiando arquivos...
if not exist "%DLL%" (
    echo ERRO: DLL nao encontrada em "%DLL%".
    exit /b 1
)
if not exist "%LAUNCHER%" (
    echo ERRO: launcher nao encontrado em "%LAUNCHER%".
    exit /b 1
)
if not exist "%SERVER%" (
    echo ERRO: servidor nao encontrado em "%SERVER%".
    exit /b 1
)

copy /Y "%DLL%" "%DESTINO%\" >NUL
if errorlevel 1 (
    echo ERRO: nao foi possivel copiar maxpayne-mp.dll.
    exit /b 1
)

copy /Y "%LAUNCHER%" "%DESTINO%\" >NUL
if errorlevel 1 (
    echo ERRO: nao foi possivel copiar maxpayne-launcher.exe.
    exit /b 1
)

echo.

tasklist /FI "IMAGENAME eq maxpayne-server.exe" 2>NUL | find /I "maxpayne-server.exe" >NUL
if not errorlevel 1 (
    taskkill /F /IM maxpayne-server.exe >NUL
)
start "" "%SERVER%"

tasklist /FI "IMAGENAME eq maxpayne.exe" 2>NUL | find /I "maxpayne.exe" >NUL
if not errorlevel 1 (
    taskkill /F /IM maxpayne.exe >NUL
)

cd /d "%DESTINO%"
"%DESTINO%\maxpayne-launcher.exe"
if errorlevel 1 (
    echo ERRO: nao foi possivel iniciar maxpayne-launcher.exe.
    exit /b 1
)

echo.
endlocal