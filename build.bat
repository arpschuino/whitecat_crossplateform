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
set OPENCV=%WC%\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib

:: Variables avec forward slashes pour le response file (g++ traite \ comme escape)
set SRC_F=%SRC:\=/%
set WC_F=%WC:\=/%
set OUT_F=%OUT:\=/%
set RTMIDI_F=%RTMIDI:\=/%
set SDL2_F=%SDL2:\=/%
set OPENCV_F=%OPENCV:\=/%

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

:: windres est dans le meme dossier que g++
set WINDRES=%GCC:g++.exe=windres.exe%
if not exist "%WINDRES%" set WINDRES=windres
echo [build] windres : %WINDRES%

goto :compile

:no_gcc
echo [ERREUR] g++ introuvable !
echo Installe MinGW dans %ROOT%\tools\MinGW\ ou C:\Program Files (x86)\CodeBlocks\MinGW\
exit /b 1

:compile
:: Le Makefile gere PCH, ressources, compilation parallele et linkage
echo [build] Compilation parallele (make -j8)...
"%ROOT%\tools\MinGW\bin\mingw32-make.exe" -j8 -C "%ROOT%"

if %ERRORLEVEL%==0 (
    echo.
    echo [build] OK : %OUT%\Whitecat_Crossplatform.exe
) else (
    echo.
    echo [build] ECHEC - voir erreurs ci-dessus
)
endlocal
exit /b %ERRORLEVEL%

:: ============================================================
:: Ancien build monolithique (conserve en secours)
:: ============================================================
:compile_legacy
if not exist "%OUT%" mkdir "%OUT%"

:: En-tete precompile (PCH) — compile une seule fois, reutilise par tous les TUs
set PCH_RSP=%OUT%\wc_pch.rsp
echo -D_GLIBCXX_USE_CXX11_ABI=0 -D_TIMESPEC_DEFINED -D__WINDOWS_MM__ -std=c++11 -g > "%PCH_RSP%"
echo -I%SDL2_F%/include >> "%PCH_RSP%"
echo -I%WC_F%/lib/windows/Cserial >> "%PCH_RSP%"
echo -I%WC_F%/lib/windows/dashard >> "%PCH_RSP%"
echo -I%WC_F%/lib/windows/enttec_pro >> "%PCH_RSP%"
echo -I%WC_F%/lib/windows/odmxusb_terry >> "%PCH_RSP%"
echo -I%WC_F%/lib/windows/compiledlibsforGCC4_8_1/include >> "%PCH_RSP%"
echo -I%SRC_F% >> "%PCH_RSP%"
echo -I%RTMIDI_F% >> "%PCH_RSP%"
echo -mwindows >> "%PCH_RSP%"
echo -x c++-header %SRC_F%/wc_tus.h >> "%PCH_RSP%"
echo -o %SRC_F%/wc_tus.h.gch >> "%PCH_RSP%"
echo [build] Compilation de l'en-tete precompile (PCH)...
"%GCC%" @"%PCH_RSP%" 2>&1
if errorlevel 1 (
    echo [AVERTISSEMENT] PCH echouee - compilation normale sans cache
) else (
    echo [build] PCH OK
)

:: Compilation des ressources (icone)
echo [build] Compilation des ressources...
set RES_OBJ=
"%WINDRES%" "%ROOT%\whitecat.rc" -O coff -o "%OUT%\whitecat_res.o"
if errorlevel 1 (
    echo [AVERTISSEMENT] windres a echoue, icone non integree
) else (
    set RES_OBJ=%OUT%\whitecat_res.o
    echo [build] Ressources OK
)

echo [build] Compilation en cours...

:: Response file pour contourner la limite de 8191 chars de CMD
set RSP=%OUT%\wc_build.rsp

echo -D_GLIBCXX_USE_CXX11_ABI=0 -D_TIMESPEC_DEFINED -D__WINDOWS_MM__ -std=c++11 -g > "%RSP%"
echo -I%SDL2_F%/include >> "%RSP%"
echo -I%WC_F%/lib/windows/Cserial >> "%RSP%"
echo -I%WC_F%/lib/windows/dashard >> "%RSP%"
echo -I%WC_F%/lib/windows/enttec_pro >> "%RSP%"
echo -I%WC_F%/lib/windows/odmxusb_terry >> "%RSP%"
echo -I%WC_F%/lib/windows/compiledlibsforGCC4_8_1/include >> "%RSP%"
echo -I%SRC_F% >> "%RSP%"
echo -I%RTMIDI_F% >> "%RSP%"
echo %SRC_F%/MAIN.cpp >> "%RSP%"
echo %SRC_F%/saveload.cpp >> "%RSP%"
echo %SRC_F%/globals.cpp >> "%RSP%"
echo %SRC_F%/display.cpp >> "%RSP%"
echo %SRC_F%/channels.cpp >> "%RSP%"
echo %SRC_F%/video.cpp >> "%RSP%"
echo %SRC_F%/ui_indexes.cpp >> "%RSP%"
echo %SRC_F%/patch.cpp >> "%RSP%"
echo %SRC_F%/audio.cpp >> "%RSP%"
echo %SRC_F%/dmx.cpp >> "%RSP%"
echo %SRC_F%/midi.cpp >> "%RSP%"
echo %SRC_F%/network.cpp >> "%RSP%"
echo %SRC_F%/chasers.cpp >> "%RSP%"
echo %SRC_F%/faders.cpp >> "%RSP%"
echo %SRC_F%/sequenciel.cpp >> "%RSP%"
echo %SRC_F%/theme.cpp >> "%RSP%"
echo %SRC_F%/banger.cpp >> "%RSP%"
echo %SRC_F%/arduino.cpp >> "%RSP%"
echo %SRC_F%/grider.cpp >> "%RSP%"
echo %SRC_F%/plot.cpp >> "%RSP%"
echo %SRC_F%/draw.cpp >> "%RSP%"
echo %SRC_F%/echo.cpp >> "%RSP%"
echo %SRC_F%/trichro.cpp >> "%RSP%"
echo %SRC_F%/schwz.cpp >> "%RSP%"
echo %SRC_F%/core.cpp >> "%RSP%"
echo %SRC_F%/midi_CORE.cpp >> "%RSP%"
echo %SRC_F%/patch_splines.cpp >> "%RSP%"
echo %SRC_F%/SmoothData.cpp >> "%RSP%"
echo %SRC_F%/grand_master.cpp >> "%RSP%"
echo %SRC_F%/grider_calcul.cpp >> "%RSP%"
echo %SRC_F%/grider_core.cpp >> "%RSP%"
echo %SRC_F%/grider_visu.cpp >> "%RSP%"
echo %SRC_F%/gestionaire_fenetres2.cpp >> "%RSP%"
echo %SRC_F%/saves_export_import.cpp >> "%RSP%"
echo %SRC_F%/faders_operations.cpp >> "%RSP%"
echo %SRC_F%/wizard_operations.cpp >> "%RSP%"
echo %SRC_F%/gui_boutons_rebuild1.cpp >> "%RSP%"
echo %SRC_F%/network_artnet_3.cpp >> "%RSP%"
echo %SRC_F%/patch_core.cpp >> "%RSP%"
echo %SRC_F%/patch_visu.cpp >> "%RSP%"
echo %SRC_F%/list_proj_visu.cpp >> "%RSP%"
echo %SRC_F%/midi_launchpad.cpp >> "%RSP%"
echo %SRC_F%/CFG_screen.cpp >> "%RSP%"
echo %SRC_F%/echo3.cpp >> "%RSP%"
echo %SRC_F%/Draw3.cpp >> "%RSP%"
echo %SRC_F%/arduino_device_core.cpp >> "%RSP%"
echo %SRC_F%/network_MAC_adress_3.cpp >> "%RSP%"
echo %SRC_F%/numpad_visuel.cpp >> "%RSP%"
echo %SRC_F%/time_visu_3.cpp >> "%RSP%"
echo %SRC_F%/trichro_visu2.cpp >> "%RSP%"
echo %SRC_F%/numpad_core.cpp >> "%RSP%"
echo %SRC_F%/time_core.cpp >> "%RSP%"
echo %SRC_F%/channels_core.cpp >> "%RSP%"
echo %RTMIDI_F%/RtMidi.cpp >> "%RSP%"
echo -o %OUT_F%/Whitecat_Crossplatform.exe >> "%RSP%"
echo -L%SDL2_F%/lib >> "%RSP%"
echo -L%WC_F%/lib/windows/enttec_pro >> "%RSP%"
echo -L%WC_F%/lib/windows/odmxusb_terry >> "%RSP%"
echo -L%WC_F%/lib/windows/compiledlibsforGCC4_8_1/x86/mingw/lib >> "%RSP%"
echo -L%WC_F%/lib/windows/compiledlibsforGCC4_8_1/lib >> "%RSP%"
echo -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer >> "%RSP%"
echo -luser32 -lgdi32 -lwsock32 -liphlpapi >> "%RSP%"
echo -lkernel32 -lcomctl32 -lwinmm -lole32 -lopengl32 >> "%RSP%"
echo %WC_F%/lib/windows/odmxusb_terry/FTD2XX.lib >> "%RSP%"
echo %WC_F%/lib/windows/enttec_pro/ftd2xx.lib >> "%RSP%"
echo -lhpdf >> "%RSP%"
echo -lz >> "%RSP%"
echo %OPENCV_F%/libopencv_calib3d248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_contrib248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_core248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_features2d248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_flann248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_gpu248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_highgui248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_imgproc248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_legacy248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_ml248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_nonfree248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_objdetect248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_ocl248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_photo248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_stitching248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_superres248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_ts248.a >> "%RSP%"
echo %OPENCV_F%/libopencv_video248.dll.a >> "%RSP%"
echo %OPENCV_F%/libopencv_videostab248.dll.a >> "%RSP%"
if defined RES_OBJ echo %RES_OBJ:\=/% >> "%RSP%"
echo -mwindows >> "%RSP%"

"%GCC%" @"%RSP%" 2>&1

if %ERRORLEVEL%==0 (
    echo.
    echo [build] OK : %OUT%\Whitecat_Crossplatform.exe
) else (
    echo.
    echo [build] ECHEC - voir erreurs ci-dessus
)

endlocal
