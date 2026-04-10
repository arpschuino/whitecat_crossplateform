@echo off
setlocal

:: ============================================================
:: Chemins relatifs au dossier du build.bat
:: ============================================================
set ROOT=%~dp0
if "%ROOT:~-1%"=="\" set ROOT=%ROOT:~0,-1%

set WC=%ROOT%\whitecatlib
set SRC=%ROOT%\Src
set OUT=%ROOT%\whitecatbuild\build\white_cat_for_mingw
set RTMIDI=%ROOT%\whitecatlib\lib\sources_of_libs\rtmidi
set SDL2=%WC%\lib\windows\sdl2

:: ============================================================
:: Prepend tous les emplacements MinGW connus en une seule ligne
:: (hors de tout bloc if pour eviter les problemes de parentheses dans PATH)
:: ============================================================
set PATH=%ROOT%\tools\MinGW\bin;C:\Program Files (x86)\CodeBlocks\MinGW\bin;C:\MinGW\bin;%PATH%

:: ============================================================
:: Detection du compilateur (if sur une seule ligne = pas de pb de parentheses)
:: ============================================================
set GCC=
if exist "%ROOT%\tools\MinGW\bin\g++.exe" set GCC=%ROOT%\tools\MinGW\bin\g++.exe
if not defined GCC if exist "C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe" set GCC=C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe
if not defined GCC if exist "C:\MinGW\bin\g++.exe" set GCC=C:\MinGW\bin\g++.exe
if not defined GCC where g++ >nul 2>&1 && set GCC=g++

if not defined GCC goto :no_gcc
echo [build] Compilateur : %GCC%
goto :compile

:no_gcc
echo [ERREUR] g++ introuvable !
echo Installe MinGW dans %ROOT%\tools\MinGW\ ou C:\Program Files (x86)\CodeBlocks\MinGW\
exit /b 1

:compile
if not exist "%OUT%" mkdir "%OUT%"
echo [build] Compilation en cours...

"%GCC%" -w -D_GLIBCXX_USE_CXX11_ABI=0 -D_TIMESPEC_DEFINED -std=c++11 -g ^
 -I%SDL2%\include ^
 -I%WC%\lib\windows\Cserial ^
 -I%WC%\lib\windows\dashard ^
 -I%WC%\lib\windows\enttec_pro ^
 -I%WC%\lib\windows\odmxusb_terry ^
 -I%WC%\lib\windows\compiledlibsforGCC4_8_1\include ^
 -I%SRC% ^
 -I%RTMIDI% ^
 %SRC%\MAIN_janv_2014_opensource_kbd.cpp ^
 %RTMIDI%\RtMidi.cpp ^
 -o "%OUT%\Whitecat_Crossplatform.exe" ^
 -L%SDL2%\lib ^
 -L%WC%\lib\windows\enttec_pro ^
 -L%WC%\lib\windows\odmxusb_terry ^
 -L%WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib ^
 -L%WC%\lib\windows\compiledlibsforGCC4_8_1\lib ^
 -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer ^
 -luser32 -lgdi32 -lwsock32 -liphlpapi ^
 -lkernel32 -lcomctl32 -lwinmm -lole32 -lopengl32 ^
 %WC%\lib\windows\odmxusb_terry\FTD2XX.lib ^
 %WC%\lib\windows\enttec_pro\ftd2xx.lib ^
 -lhpdf ^
 -lz ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_calib3d248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_contrib248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_core248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_features2d248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_flann248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_gpu248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_highgui248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_imgproc248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_legacy248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_ml248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_nonfree248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_objdetect248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_ocl248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_photo248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_stitching248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_superres248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_ts248.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_video248.dll.a ^
 %WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_videostab248.dll.a ^
 -mwindows 2>&1

if %ERRORLEVEL%==0 (
    echo.
    echo [build] OK : %OUT%\Whitecat_Crossplatform.exe
) else (
    echo.
    echo [build] ECHEC - voir erreurs ci-dessus
)

endlocal
