#!/bin/bash
# ============================================================
# WhiteCat — Préparation du build macOS (à lancer UNE fois)
# ------------------------------------------------------------
#   1. vérifie les Command Line Tools Xcode et Homebrew
#   2. installe les bibliothèques via Homebrew
#   3. télécharge les fichiers d'exécution (Fonts/, gfx/, user/…)
#      depuis l'archive Linux de la release et les copie à côté
#      de l'exécutable (whitecatbuild/build/macos/)
#
# Ensuite : make -f Makefile.macos -j8
#
# N'écrase jamais un fichier existant (user/ et saves/ conservés).
# Relançable sans risque (idempotent).
# ============================================================

set -eu

WC_RELEASE=v0.9.1
WC_ARCHIVE=Whitecat_Crossplatform_0.9.1_linux_x86_64
WC_URL=https://github.com/arpschuino/whitecat_crossplateform/releases/download/$WC_RELEASE/$WC_ARCHIVE.tar.gz
WC_RUNTIME_DIRS="Fonts audio gfx import_export plans ressources saves user"

ROOT="$(cd "$(dirname "$0")" && pwd)"
OUT="$ROOT/whitecatbuild/build/macos"

# --- 1. Outils -------------------------------------------------
if ! xcode-select -p >/dev/null 2>&1; then
    echo "[setup] Command Line Tools Xcode absents : lancez 'xcode-select --install' puis relancez ce script."
    exit 1
fi
if ! command -v brew >/dev/null 2>&1; then
    echo "[setup] Homebrew absent : voir https://brew.sh puis relancez ce script."
    exit 1
fi

# --- 2. Bibliothèques -----------------------------------------
echo "[setup] Installation des bibliothèques Homebrew..."
brew install pkg-config sdl2 sdl2_ttf sdl2_image sdl2_mixer libharu rtmidi

# --- 3. Fichiers d'exécution ----------------------------------
mkdir -p "$OUT"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

echo "[setup] Téléchargement des fichiers d'exécution ($WC_RELEASE)..."
curl -fL --progress-bar -o "$TMP/runtime.tar.gz" "$WC_URL"
tar xzf "$TMP/runtime.tar.gz" -C "$TMP"

for d in $WC_RUNTIME_DIRS; do
    if [ -d "$TMP/$WC_ARCHIVE/$d" ]; then
        cp -Rn "$TMP/$WC_ARCHIVE/$d" "$OUT/" 2>/dev/null || true
    fi
done

echo
echo "[setup] OK. Compilez avec :  make -f Makefile.macos -j8"
echo "        puis lancez :        cd whitecatbuild/build/macos && ./Whitecat_Crossplatform"
