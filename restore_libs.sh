#!/bin/bash
# Copie toutes les libs dont le binaire a besoin (via ldd), sauf les libs système de base
BINARY="/mnt/c/Nextcloud/ARPSCHUINO_PARTAGE/codes/whitecat_crossplateform/whitecatbuild/build/linux/Whitecat_Crossplatform"
DEST="/mnt/c/Nextcloud/ARPSCHUINO_PARTAGE/codes/whitecat_crossplateform/whitecatbuild/release/Whitecat_Crossplatform_0.9.1_linux_x86_64/lib"

# Libs à ne PAS bundler (présentes sur tout système Linux)
SKIP="linux-vdso|libpthread|libm\.so|libdl\.so|libc\.so|ld-linux"

mkdir -p "$DEST"

ldd "$BINARY" | while read line; do
    # format: "libfoo.so.X => /path/to/libfoo.so.X (0x...)"
    path=$(echo "$line" | awk '{print $3}')
    name=$(echo "$line" | awk '{print $1}')
    if [ -f "$path" ] && ! echo "$name" | grep -qE "$SKIP"; then
        cp -L "$path" "$DEST/$name" && echo "OK $name"
    fi
done
