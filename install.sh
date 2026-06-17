#!/bin/bash
# ============================================================
# WhiteCat — Intégration bureau Linux (à lancer UNE fois)
# ------------------------------------------------------------
# Installe, pour l'utilisateur courant :
#   - l'icône du chat (barre des tâches / menu)
#   - un raccourci dans le menu d'applications + sur le bureau
#   - sous Wayland/labwc (Raspberry Pi) : une "window rule" qui place
#     la fenêtre en haut-gauche (l'app ne peut pas se positionner seule
#     sous Wayland)
#
# Ne touche qu'au profil utilisateur (~/.local, ~/.config) — pas de sudo.
# Relançable sans risque (idempotent).
# ============================================================

set -u

# Position voulue pour la fenêtre (modifiable) :
WC_POS_X=0
WC_POS_Y=0

# Dossier de ce script = dossier de WhiteCat
WCDIR="$(cd "$(dirname "$0")" && pwd)"
EXE="$WCDIR/Whitecat_Crossplatform"
ICON_SRC="$WCDIR/gfx/logo.png"

if [ ! -f "$EXE" ]; then
    echo "ERREUR : Whitecat_Crossplatform introuvable dans $WCDIR"
    echo "Lance ce script depuis le dossier de WhiteCat."
    exit 1
fi

echo "Installation de WhiteCat depuis : $WCDIR"

# --- 1. Icône -------------------------------------------------
ICON_DIR="$HOME/.local/share/icons/hicolor/256x256/apps"
mkdir -p "$ICON_DIR"
if [ -f "$ICON_SRC" ]; then
    cp -f "$ICON_SRC" "$ICON_DIR/whitecat.png"
    echo "  [ok] icône installée"
else
    echo "  [!] gfx/logo.png introuvable — icône ignorée"
fi

# --- 2. Raccourci menu d'applications -------------------------
APPDIR="$HOME/.local/share/applications"
mkdir -p "$APPDIR"
DESKTOP_FILE="$APPDIR/whitecat.desktop"
cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=WhiteCat
Comment=Stage lighting console
Exec=bash -c 'cd "$WCDIR" && ./Whitecat_Crossplatform'
Icon=whitecat
Terminal=false
Categories=AudioVideo;
StartupWMClass=whitecat
EOF
chmod +x "$DESKTOP_FILE"
echo "  [ok] raccourci dans le menu d'applications"

# --- 3. Raccourci sur le bureau ------------------------------
for DESK in "$HOME/Bureau" "$HOME/Desktop"; do
    if [ -d "$DESK" ]; then
        cp -f "$DESKTOP_FILE" "$DESK/whitecat.desktop"
        chmod +x "$DESK/whitecat.desktop"
        gio set "$DESK/whitecat.desktop" metadata::trusted true 2>/dev/null || true
        echo "  [ok] raccourci sur le bureau ($DESK)"
    fi
done

# --- 4. Rafraîchissement des caches --------------------------
update-desktop-database "$APPDIR" 2>/dev/null || true
gtk-update-icon-cache -f "$HOME/.local/share/icons/hicolor" 2>/dev/null || true

# --- 5. Placement de la fenêtre sous Wayland/labwc -----------
# Sous Wayland, l'application ne peut PAS se positionner (le code
# l'ignore) — c'est le compositeur qui place. On ajoute donc une règle
# labwc basée sur l'app_id "whitecat" (défini par WhiteCat).
if command -v labwc >/dev/null 2>&1; then
    RC="$HOME/.config/labwc/rc.xml"
    mkdir -p "$HOME/.config/labwc"
    if [ ! -f "$RC" ] && [ -f /etc/xdg/labwc/rc.xml ]; then
        cp /etc/xdg/labwc/rc.xml "$RC"
    fi
    if [ -f "$RC" ] && grep -q '<windowRules>' "$RC" && ! grep -q 'identifier="whitecat"' "$RC"; then
        sed -i "s#<windowRules>#<windowRules>\n    <windowRule identifier=\"whitecat\"><action name=\"MoveTo\" x=\"$WC_POS_X\" y=\"$WC_POS_Y\"/></windowRule>\n    <windowRule title=\"WhiteCat\"><action name=\"MoveTo\" x=\"$WC_POS_X\" y=\"$WC_POS_Y\"/></windowRule>#" "$RC"
        echo "  [ok] window rule labwc ajoutée (fenêtre en $WC_POS_X,$WC_POS_Y)"
        kill -SIGHUP "$(pidof labwc)" 2>/dev/null || true
    elif [ -f "$RC" ] && grep -q 'identifier="whitecat"' "$RC"; then
        echo "  [ok] window rule labwc déjà présente"
    fi
fi

echo ""
echo "Terminé. Lance WhiteCat depuis le menu d'applications ou le raccourci du bureau."
echo "(Le binaire seul garde une icône générique — c'est normal sous Linux.)"
