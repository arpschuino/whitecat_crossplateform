@echo off
setlocal

:: ============================================================
:: WhiteCat - installation du kit de build Windows
:: Installe (sans jamais ecraser un fichier existant) :
::   tools\MinGW\                        compilateur TDM-GCC 5.1.0 (32 bit)
::   whitecatlib\                        bibliotheques (SDL2, RtMidi, OpenCV, libharu, FTDI...)
::   whitecatbuild\build\white_cat_for_mingw\   DLL + polices + ressources + config par defaut
:: Ensuite : build.bat (ou Ctrl+Shift+B dans VSCode).
:: ============================================================

set KIT_NAME=whitecat-windows-buildkit-v1.zip
set KIT_URL=https://github.com/arpschuino/whitecat_crossplateform/releases/download/buildkit-windows-v1/%KIT_NAME%

set ROOT=%~dp0
if "%ROOT:~-1%"=="\" set ROOT=%ROOT:~0,-1%
set TMPKIT=%TEMP%\whitecat_buildkit

if exist "%ROOT%\tools\MinGW\bin\g++.exe" if exist "%ROOT%\whitecatlib\lib\windows\sdl2" (
    echo [setup] Kit deja installe - complement des fichiers manquants uniquement.
)

:: 1. Le kit : a cote de ce script, sinon telechargement
set KIT_ZIP=%ROOT%\%KIT_NAME%
if exist "%KIT_ZIP%" (
    echo [setup] Kit trouve : %KIT_ZIP%
) else (
    echo [setup] Telechargement du kit ^(~100 Mo^) :
    echo         %KIT_URL%
    powershell -NoProfile -ExecutionPolicy Bypass -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; $ProgressPreference='SilentlyContinue'; Invoke-WebRequest -Uri '%KIT_URL%' -OutFile '%KIT_ZIP%'"
    if not exist "%KIT_ZIP%" goto :download_failed
)

:: 2. Extraction dans un dossier temporaire
echo [setup] Extraction...
if exist "%TMPKIT%" rmdir /s /q "%TMPKIT%"
powershell -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -LiteralPath '%KIT_ZIP%' -DestinationPath '%TMPKIT%' -Force"
if errorlevel 1 goto :extract_failed

:: 3. Copie sans ecraser (/XC /XN /XO : fichiers existants conserves - user\, saves\ intacts)
echo [setup] Installation des fichiers manquants...
robocopy "%TMPKIT%" "%ROOT%" /E /XC /XN /XO /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 goto :copy_failed
rmdir /s /q "%TMPKIT%"

if not exist "%ROOT%\tools\MinGW\bin\g++.exe" goto :copy_failed
echo.
echo [setup] OK. Pour compiler : build.bat  (ou Ctrl+Shift+B dans VSCode)
echo         L'exe sera dans whitecatbuild\build\white_cat_for_mingw\
exit /b 0

:download_failed
echo [ERREUR] Telechargement impossible. Telechargez a la main :
echo          %KIT_URL%
echo          et placez %KIT_NAME% a cote de setup_windows.bat, puis relancez.
exit /b 1

:extract_failed
echo [ERREUR] Extraction du kit impossible (zip corrompu ?). Supprimez %KIT_ZIP% et relancez.
exit /b 1

:copy_failed
echo [ERREUR] Installation incomplete (copie des fichiers). Voir les messages ci-dessus.
exit /b 1
