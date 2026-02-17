#!/bin/bash
# Build Geo Checker C++
set -e
cd "$(dirname "$0")"

# Prefer system cmake, fallback to pip
CMAKE=""
for c in cmake cmake3; do
    if command -v $c &>/dev/null; then CMAKE=$c; break; fi
done
if [ -z "$CMAKE" ] && [ -f ../venv/bin/cmake ]; then
    CMAKE="../venv/bin/cmake"
fi
if [ -z "$CMAKE" ]; then
    echo "CMake not found. Install: sudo apt install cmake"
    echo "Or: pip install cmake"
    exit 1
fi

echo "=== Geo Checker C++ Build ==="
mkdir -p build
cd build
$CMAKE .. -DCMAKE_BUILD_TYPE=Release
$CMAKE --build . -j$(nproc 2>/dev/null || echo 4)

echo ""
echo "Done! Binaries in $(pwd)/"
[ -f geochecker-cli ] && echo "  CLI:  ./geochecker-cli search <geosite.dat> <domain>"
[ -f geochecker ]     && echo "  GUI:  ./geochecker"
