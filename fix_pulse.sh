#!/bin/bash
LIBDIR="/mnt/c/Nextcloud/ARPSCHUINO_PARTAGE/codes/whitecat_crossplateform/whitecatbuild/release/Whitecat_Crossplatform_0.9.1_linux_x86_64/lib"
patchelf --set-rpath '$ORIGIN' "$LIBDIR/libpulse.so.0"
patchelf --set-rpath '$ORIGIN' "$LIBDIR/libpulse-simple.so.0"
echo "Apres patch :"
readelf -d "$LIBDIR/libpulse.so.0" | grep -E 'RPATH|RUNPATH'
readelf -d "$LIBDIR/libpulse-simple.so.0" | grep -E 'RPATH|RUNPATH'
