@echo off
set DESTINO=C:\Users\mateus-bsod\Downloads\MaxPayne_Win_EN_Download\MaxPayne_Win_EN\Game Files\maxpayne-mp

if not exist "%DESTINO%" mkdir "%DESTINO%"

echo Copiando arquivos...
copy "Release\maxpayne-mp.dll" "%DESTINO%\"
copy "Release\maxpayne-injector.exe" "%DESTINO%\"

echo.

tasklist /FI "IMAGENAME eq maxpayne-server.exe" 2>NUL | find /I "maxpayne-server.exe" >NUL
if not errorlevel 1 (
    taskkill /F /IM maxpayne-server.exe >NUL
)
start "" "Release/maxpayne-server.exe"

tasklist /FI "IMAGENAME eq MaxPayne.exe" 2>NUL | find /I "MaxPayne.exe" >NUL
if not errorlevel 1 (
    taskkill /F /IM MaxPayne.exe >NUL
)

cd /d "%DESTINO%"
start "" "maxpayne-injector.exe"

echo.