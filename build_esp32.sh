#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/embedded/main"
ARDUINO_CLI="${ARDUINO_CLI:-/home/szilard/bin/arduino-cli}"
FQBN="esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=enabled"
BUILD_DIR="$SCRIPT_DIR/build/esp32"

usage() {
    echo "Usage: $0 [-f FQBN] [-o OUTPUT_DIR]"
    echo "  -f  Board FQBN (default: $FQBN)"
    echo "  -o  Output directory (default: $BUILD_DIR)"
    exit 1
}

while getopts "f:o:h" opt; do
    case $opt in
        f) FQBN="$OPTARG" ;;
        o) BUILD_DIR="$OPTARG" ;;
        *) usage ;;
    esac
done

echo "==> Building sketch: $SKETCH_DIR"
echo "==> FQBN: $FQBN"
echo "==> Output: $BUILD_DIR"

mkdir -p "$BUILD_DIR"

"$ARDUINO_CLI" compile \
    --fqbn "$FQBN" \
    --output-dir "$BUILD_DIR" \
    --warnings default \
    "$SKETCH_DIR"

VERSION=$(sed -nE 's/^#define[[:space:]]+FW_VERSION[[:space:]]+"([^"]+)".*/\1/p' "$SKETCH_DIR/Version.hpp")
if [[ -z "$VERSION" ]]; then
    echo "Failed to read FW_VERSION from $SKETCH_DIR/Version.hpp" >&2
    exit 1
fi

BINARY="$BUILD_DIR/main.ino.bin"
if [[ -f "$BINARY" ]]; then
    RENAMED="$BUILD_DIR/ESP32_${VERSION}.bin"
    mv "$BINARY" "$RENAMED"
    echo ""
    echo "==> Output binary: $RENAMED"
else
    echo ""
    echo "==> Build artifacts:"
    ls -lh "$BUILD_DIR"
fi
