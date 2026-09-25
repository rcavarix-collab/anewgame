#!/bin/sh
# Cross-compiles the game for Windows with MinGW-w64, to catch compile and
# link errors before the owner opens Visual Studio. It is not the owner's
# build (MSVC is), and it never runs the game: a clean result means "it
# builds", nothing more. Sources come from the project file, the list of record.
# Usage: tools/check_mingw.sh   (from anywhere; exit 1 on failure)
# Needs: apt-get install mingw-w64
set -e
cd "$(dirname "$0")/.."
PROJ=$(ls *.vcxproj | head -n 1)
SRCS=$(grep -o 'ClCompile Include="[^"]*"' "$PROJ" | sed 's/.*="//;s/"//')
OUT="${TMPDIR:-/tmp}/walkgrid_mingw"
mkdir -p "$OUT"
# xaudio2_8: the XAudio2 import library MinGW ships (MSVC links 2.9); same API.
if x86_64-w64-mingw32-g++-posix -std=c++17 -O1 -Wall -Wno-unknown-pragmas \
    -DNOMINMAX -DUNICODE -D_UNICODE -municode -mwindows $SRCS -o "$OUT/game.exe" -static \
    -ld3d11 -ldxgi -ld3dcompiler -lxaudio2_8 -lole32 -lgdiplus -lwinmm -lshell32 -luuid \
    > "$OUT/build.log" 2>&1; then
    WARN=$(grep -c "warning:" "$OUT/build.log" || true)
    echo "ok   MinGW cross-compile builds ($WARN warnings; log: $OUT/build.log)"
else
    grep -E "error|undefined reference" "$OUT/build.log" | head -n 30
    echo "FAIL MinGW cross-compile (log: $OUT/build.log)"
    exit 1
fi
