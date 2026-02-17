#!/bin/bash
# Installs icon and .desktop for Geo Checker into ~/.local/share/
# After this, the dock/launcher icon will be correct.
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RESOURCES="$SCRIPT_DIR/resources"
ICON_ICO="$RESOURCES/app_icon.ico"
BUILD_DIR="$SCRIPT_DIR/build"
EXE="$BUILD_DIR/geochecker"
LOCAL_SHARE="${XDG_DATA_HOME:-$HOME/.local/share}"
ICONS_DIR="$LOCAL_SHARE/icons/hicolor"
APPS_DIR="$LOCAL_SHARE/applications"

if [ ! -f "$ICON_ICO" ]; then
    echo "Not found: $ICON_ICO"
    exit 1
fi

# Convert ICO -> PNG for icon theme (Linux expects PNG)
PNG_DIR="$SCRIPT_DIR/build/icon-png"
mkdir -p "$PNG_DIR"
PNG_256="$PNG_DIR/geochecker-256.png"

if command -v python3 &>/dev/null; then
    if python3 -c "from PIL import Image" 2>/dev/null; then
        python3 "$RESOURCES/ico_to_png.py" "$ICON_ICO" "$PNG_256"
    else
        echo "Install Pillow to convert icon: pip install Pillow"
        echo "Or copy app_icon.ico manually to $ICONS_DIR/256x256/apps/geochecker.ico"
        exit 2
    fi
else
    echo "python3 not found"
    exit 2
fi

# Install icon into hicolor theme
mkdir -p "$ICONS_DIR/256x256/apps"
cp "$PNG_256" "$ICONS_DIR/256x256/apps/geochecker.png"
echo "Icon: $ICONS_DIR/256x256/apps/geochecker.png"

# .desktop with path to binary
mkdir -p "$APPS_DIR"
DESKTOP="$APPS_DIR/geochecker.desktop"
cat > "$DESKTOP" << DESK
[Desktop Entry]
Version=1.0
Type=Application
Name=Geo Checker
Comment=Geo Checker application
Exec=$EXE
Icon=geochecker
StartupWMClass=geochecker
Terminal=false
Categories=Utility;
DESK
echo "Desktop: $DESKTOP"

# Update icon and .desktop caches
if command -v gtk-update-icon-cache &>/dev/null; then
    gtk-update-icon-cache -f -t "$ICONS_DIR" 2>/dev/null || true
fi
if command -v update-desktop-database &>/dev/null; then
    update-desktop-database "$APPS_DIR" 2>/dev/null || true
fi

echo "Done. Run the app from the menu/dock or: $EXE"
