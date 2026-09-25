#!/bin/sh
# Builds and runs the native tests (host compiler; no Windows needed).
# Usage: tests/run.sh   (from anywhere)
set -e
cd "$(dirname "$0")"
sh ../tools/check_msvc.sh   # what Visual Studio's SDL checks reject and GCC doesn't
python3 ../tools/check_layers.py   # the layer rule (docs/FOUNDATIONS.md 2)
# No player-visible trace of the old name, in any case (a case-sensitive
# search missed the title screen's "VOXISTICS" once).
if grep -in '"[^"]*voxistics' ../*.cpp ../*.h; then echo "FAIL: the old name in a string"; exit 1; fi
OUT="${TMPDIR:-/tmp}/walkgrid_tests"
g++ -std=c++17 -O1 -Wall -Wextra -pthread -Istub -I.. \
    tests.cpp ../world.cpp ../terrain.cpp ../jobs.cpp ../glowlight.cpp ../worldfile.cpp ../vtex.cpp ../blocktex.cpp ../icons.cpp \
    ../music_synth.cpp ../sfx_synth.cpp ../soundscape.cpp ../facetmesh.cpp ../groundmesh.cpp ../collide.cpp \
    -o "$OUT"
"$OUT"
