#!/usr/bin/env python3
"""Convert app_icon.ico to PNG for Linux (hicolor). Requires: pip install Pillow"""
import sys
from pathlib import Path

def main():
    if len(sys.argv) < 3:
        print("Usage: ico_to_png.py <input.ico> <output.png>", file=sys.stderr)
        sys.exit(1)
    try:
        from PIL import Image
    except ImportError:
        print("Pillow not found. Install: pip install Pillow", file=sys.stderr)
        sys.exit(2)
    src = Path(sys.argv[1])
    dst = Path(sys.argv[2])
    if not src.exists():
        print(f"Not found: {src}", file=sys.stderr)
        sys.exit(3)
    img = Image.open(src)
    # ICO may contain multiple sizes; resize to 256 for hicolor
    img = img.resize((256, 256), Image.Resampling.LANCZOS)
    dst.parent.mkdir(parents=True, exist_ok=True)
    img.save(dst, "PNG")
    print(f"Saved {dst}")

if __name__ == "__main__":
    main()
