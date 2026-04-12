@echo off
set PATH=C:\Program Files (x86)\CodeBlocks\MinGW\bin;%PATH%
set WC=C:\whitecat_crossplateform
set SRC=D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\Src
set OUT=D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\whitecatbuild\build\white_cat_for_mingw
set RTMIDI=D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\whitecatlib\lib\sources_of_libs\rtmidi
set SDL2=%WC%\whitecatlib\lib\windows\sdl2

echo [BUILD SDL2 - WhiteCat]
echo Compilation...

"C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe" ^
 -D_GLIBCXX_USE_CXX11_ABI=0 -D_TIMESPEC_DEFINED -std=c++11 ^
 -I%SDL2%\include ^
 -I%WC%\whitecatlib\lib\windows\odmxusb_terry ^
 -I%WC%\whitecatlib\lib\windows\dashard ^
 -I%WC%\whitecatlib\lib\windows\enttec_pro ^
 -I%WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\include ^
 -I%SRC% ^
 -IC:\MinGW\include ^
 -I%RTMIDI% ^
 %SRC%\MAIN.cpp ^
 %RTMIDI%\RtMidi.cpp ^
 -o %OUT%\Whitecat_Crossplatform.exe ^
 -L%SDL2%\lib ^
 -L%WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib ^
 -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -lSDL2_mixer ^
 -luser32 -lgdi32 -lwsock32 -liphlpapi ^
 -lkernel32 -lcomctl32 -lwinmm -lole32 ^
 -mwindows

if %ERRORLEVEL% == 0 (
    echo.
    echo BUILD OK - %OUT%\Whitecat_Crossplatform.exe
) else (
    echo.
    echo BUILD FAILED - errorlevel=%ERRORLEVEL%
)
