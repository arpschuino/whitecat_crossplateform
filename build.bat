@echo off
set PATH=C:\Program Files (x86)\CodeBlocks\MinGW\bin;C:\MinGW\bin;%PATH%
set WC=C:\whitecat_crossplateform
set SRC=D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\Src
set OUT=D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\whitecatbuild\build\white_cat_for_mingw
set RTMIDI=D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\whitecatlib\lib\sources_of_libs\rtmidi
set SDL2=%WC%\whitecatlib\lib\windows\sdl2

"C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe" -Wall -D_GLIBCXX_USE_CXX11_ABI=0 -D_TIMESPEC_DEFINED -std=c++11 -g ^
 -I%SDL2%\include ^
 -I%WC%\whitecatlib\lib\windows\Cserial ^
 -I%WC%\whitecatlib\lib\windows\dashard ^
 -I%WC%\whitecatlib\lib\windows\enttec_pro ^
 -I%WC%\whitecatlib\lib\windows\odmxusb_terry ^
 -I%WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\include ^
 -I%SRC% ^
 -IC:\MinGW\include ^
 -I%RTMIDI% ^
 %SRC%\MAIN_janv_2014_opensource_kbd.cpp ^
 %RTMIDI%\RtMidi.cpp ^
 -o %OUT%\Whitecat_Crossplatform.exe ^
 -L%SDL2%\lib ^
 -L%WC%\whitecatlib\lib\windows\enttec_pro ^
 -L%WC%\whitecatlib\lib\windows\odmxusb_terry ^
 -L%WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib ^
 -L%WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\lib ^
 -LC:\MinGW\lib ^
 -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer ^
 -luser32 -lgdi32 -lwsock32 -liphlpapi ^
 -lkernel32 -lcomctl32 -lwinmm -lole32 -lopengl32 ^
 %WC%\whitecatlib\lib\windows\odmxusb_terry\FTD2XX.lib ^
 %WC%\whitecatlib\lib\windows\enttec_pro\ftd2xx.lib ^
 -lhpdf ^
 -lz ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_calib3d248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_contrib248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_core248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_features2d248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_flann248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_gpu248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_highgui248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_imgproc248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_legacy248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_ml248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_nonfree248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_objdetect248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_ocl248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_photo248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_stitching248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_superres248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_ts248.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_video248.dll.a ^
 %WC%\whitecatlib\lib\windows\compiledlibsforGCC4_8_1\x86\mingw\lib\libopencv_videostab248.dll.a ^
 -mwindows 2>&1
