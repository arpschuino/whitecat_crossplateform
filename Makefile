# ============================================================
# WhiteCat - Build parallele (MinGW / mingw32-make)
# Usage : mingw32-make -j8
#         mingw32-make clean
# ============================================================

# Chemins absolus pour eviter les problemes de repertoire courant
# $(CURDIR) est fourni par make = repertoire courant au lancement
CURDIR_F := $(subst \,/,$(CURDIR))

WC      := $(CURDIR_F)/whitecatlib
SRC     := $(CURDIR_F)/Src
OUT     := $(CURDIR_F)/whitecatbuild/build/white_cat_for_mingw
RTMIDI  := $(WC)/lib/sources_of_libs/rtmidi
SDL2    := $(WC)/lib/windows/sdl2
OPENCV  := $(WC)/lib/windows/compiledlibsforGCC4_8_1/x86/mingw/lib

GCC     := $(CURDIR_F)/tools/MinGW/bin/g++.exe
WINDRES := $(CURDIR_F)/tools/MinGW/bin/windres.exe

# TEMP court (ex: UTILIS~1) fait echouer as.exe sur les gros objets
TMPDIR  := $(OUT)/tmp
export TEMP := $(subst /,\,$(TMPDIR))
export TMP  := $(subst /,\,$(TMPDIR))

# Assure que tools/MinGW/bin est dans le PATH (DLLs pour mingw32/bin/as.exe)
export PATH := $(subst /,\,$(CURDIR_F)/tools/MinGW/bin);$(PATH)

CFLAGS  := -D_GLIBCXX_USE_CXX11_ABI=0 -D_TIMESPEC_DEFINED -D__WINDOWS_MM__ \
            -std=c++11 -g -mwindows

INCS    := -I$(SDL2)/include \
            -I$(WC)/lib/windows/Cserial \
            -I$(WC)/lib/windows/dashard \
            -I$(WC)/lib/windows/enttec_pro \
            -I$(WC)/lib/windows/odmxusb_terry \
            -I$(WC)/lib/windows/compiledlibsforGCC4_8_1/include \
            -I$(SRC) \
            -I$(RTMIDI)

LDFLAGS := -mwindows \
            -L$(SDL2)/lib \
            -L$(WC)/lib/windows/enttec_pro \
            -L$(WC)/lib/windows/odmxusb_terry \
            -L$(WC)/lib/windows/compiledlibsforGCC4_8_1/x86/mingw/lib \
            -L$(WC)/lib/windows/compiledlibsforGCC4_8_1/lib \
            -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer \
            -luser32 -lgdi32 -lwsock32 -liphlpapi \
            -lkernel32 -lcomctl32 -lwinmm -lole32 -lopengl32 \
            $(WC)/lib/windows/odmxusb_terry/FTD2XX.lib \
            $(WC)/lib/windows/enttec_pro/ftd2xx.lib \
            -lhpdf -lz \
            $(OPENCV)/libopencv_calib3d248.dll.a \
            $(OPENCV)/libopencv_contrib248.dll.a \
            $(OPENCV)/libopencv_core248.dll.a \
            $(OPENCV)/libopencv_features2d248.dll.a \
            $(OPENCV)/libopencv_flann248.dll.a \
            $(OPENCV)/libopencv_gpu248.dll.a \
            $(OPENCV)/libopencv_highgui248.dll.a \
            $(OPENCV)/libopencv_imgproc248.dll.a \
            $(OPENCV)/libopencv_legacy248.dll.a \
            $(OPENCV)/libopencv_ml248.dll.a \
            $(OPENCV)/libopencv_nonfree248.dll.a \
            $(OPENCV)/libopencv_objdetect248.dll.a \
            $(OPENCV)/libopencv_ocl248.dll.a \
            $(OPENCV)/libopencv_photo248.dll.a \
            $(OPENCV)/libopencv_stitching248.dll.a \
            $(OPENCV)/libopencv_superres248.dll.a \
            $(OPENCV)/libopencv_ts248.a \
            $(OPENCV)/libopencv_video248.dll.a \
            $(OPENCV)/libopencv_videostab248.dll.a

# ============================================================
# Sources
# ============================================================

SRC_CPPS := \
    $(SRC)/MAIN.cpp \
    $(SRC)/saveload.cpp \
    $(SRC)/globals.cpp \
    $(SRC)/display.cpp \
    $(SRC)/channels.cpp \
    $(SRC)/video.cpp \
    $(SRC)/ui_indexes.cpp \
    $(SRC)/patch.cpp \
    $(SRC)/audio.cpp \
    $(SRC)/dmx.cpp \
    $(SRC)/midi.cpp \
    $(SRC)/network.cpp \
    $(SRC)/chasers.cpp \
    $(SRC)/faders.cpp \
    $(SRC)/sequenciel.cpp \
    $(SRC)/theme.cpp \
    $(SRC)/banger.cpp \
    $(SRC)/arduino.cpp \
    $(SRC)/grider.cpp \
    $(SRC)/plot.cpp \
    $(SRC)/draw.cpp \
    $(SRC)/echo.cpp \
    $(SRC)/trichro.cpp \
    $(SRC)/schwz.cpp \
    $(SRC)/core.cpp \
    $(SRC)/midi_CORE.cpp \
    $(SRC)/patch_splines.cpp \
    $(SRC)/SmoothData.cpp \
    $(SRC)/grand_master.cpp \
    $(SRC)/grider_calcul.cpp \
    $(SRC)/grider_core.cpp \
    $(SRC)/grider_visu.cpp \
    $(SRC)/gestionaire_fenetres2.cpp \
    $(SRC)/saves_export_import.cpp \
    $(SRC)/saves_menu.cpp \
    $(SRC)/banger_visu.cpp \
    $(SRC)/chasers_visu.cpp \
    $(SRC)/channels_visu.cpp \
    $(SRC)/sequentiel_visu.cpp \
    $(SRC)/trichro_core.cpp \
    $(SRC)/faders_core.cpp \
    $(SRC)/arduino_core.cpp \
    $(SRC)/arduino_visu.cpp \
    $(SRC)/midi_functions.cpp \
    $(SRC)/CFG_config_panel.cpp \
    $(SRC)/faders_operations.cpp \
    $(SRC)/minifaders_visu.cpp \
    $(SRC)/minifaders_core.cpp \
    $(SRC)/faders_visuels.cpp \
    $(SRC)/sequentiel_core.cpp \
    $(SRC)/keyboard_functions2.cpp \
    $(SRC)/wizard_operations.cpp \
    $(SRC)/wizard.cpp \
    $(SRC)/logicals_intres.cpp \
    $(SRC)/saves_export_pdf2.cpp \
    $(SRC)/save_show.cpp \
    $(SRC)/list_proj_core.cpp \
    $(SRC)/gui_boutons_rebuild1.cpp \
    $(SRC)/network_artnet_3.cpp \
    $(SRC)/patch_core.cpp \
    $(SRC)/patch_visu.cpp \
    $(SRC)/list_proj_visu.cpp \
    $(SRC)/midi_launchpad.cpp \
    $(SRC)/CFG_screen.cpp \
    $(SRC)/echo3.cpp \
    $(SRC)/Draw3.cpp \
    $(SRC)/arduino_device_core.cpp \
    $(SRC)/network_MAC_adress_3.cpp \
    $(SRC)/numpad_visuel.cpp \
    $(SRC)/time_visu_3.cpp \
    $(SRC)/trichro_visu2.cpp \
    $(SRC)/numpad_core.cpp \
    $(SRC)/time_core.cpp \
    $(SRC)/channels_core.cpp \
    $(SRC)/Call_everybody_5.cpp \
    $(SRC)/video_tracking_core.cpp \
    $(SRC)/video_tracking_visu.cpp \
    $(SRC)/procs_visuels_rebuild1.cpp \
    $(SRC)/dmx_functions.cpp \
    $(SRC)/chasers_core.cpp

RTMIDI_CPP := $(RTMIDI)/RtMidi.cpp

# ============================================================
# Objets et dependances
# ============================================================

OBJS := $(SRC_CPPS:$(SRC)/%.cpp=$(OUT)/%.o) $(OUT)/RtMidi.o
DEPS := $(OBJS:.o=.d)

PCH  := $(SRC)/wc_tus.h.gch
RES  := $(OUT)/whitecat_res.o
EXE  := $(OUT)/Whitecat_Crossplatform.exe

# ============================================================
# Cibles
# ============================================================

.PHONY: all clean pch dirs

all: dirs $(PCH) $(EXE)
	@echo.
	@echo [OK] $(EXE)

# Linkage
$(EXE): $(OBJS) $(RES)
	@echo [link] Whitecat_Crossplatform.exe ...
	$(GCC) $^ $(LDFLAGS) -o $@

# En-tete precompile
$(PCH): $(SRC)/wc_tus.h
	@echo [pch] wc_tus.h ...
	$(GCC) $(CFLAGS) $(INCS) -x c++-header $< -o $@

pch: dirs $(PCH)

# Ressources Windows
$(RES): $(CURDIR_F)/whitecat.rc
	@echo [rc] whitecat.rc
	-$(WINDRES) $< -O coff -o $@

# Compilation SRC (avec dependances automatiques -MMD)
$(OUT)/%.o: $(SRC)/%.cpp $(PCH)
	@echo [cc] $(<F)
	$(GCC) $(CFLAGS) $(INCS) -MMD -MP -c $< -o $@

# RtMidi (pas de PCH)
$(OUT)/RtMidi.o: $(RTMIDI_CPP)
	@echo [cc] RtMidi.cpp
	$(GCC) $(CFLAGS) $(INCS) -MMD -MP -c $< -o $@

# Creation des dossiers de sortie
dirs:
	if not exist "$(subst /,\,$(OUT))" mkdir "$(subst /,\,$(OUT))"
	if not exist "$(subst /,\,$(TMPDIR))" mkdir "$(subst /,\,$(TMPDIR))"

# Nettoyage
clean:
	-del /q "$(subst /,\,$(OUT))\*.o" 2>nul
	-del /q "$(subst /,\,$(OUT))\*.d" 2>nul
	-del /q "$(subst /,\,$(SRC))\wc_tus.h.gch" 2>nul
	@echo [clean] OK

# Dependances auto-generees (incremental builds)
-include $(DEPS)
