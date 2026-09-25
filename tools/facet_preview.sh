#!/bin/sh
# Builds and runs tools/facet_preview.cpp (host compiler; no Windows needed):
# still pictures of the faceted ground (M1.2).
# Usage: tools/facet_preview.sh OUTDIR [view]
set -e
cd "$(dirname "$0")"
OUT="${TMPDIR:-/tmp}/walkgrid_facet_preview"
g++ -std=c++17 -O2 -Wall -Wextra -pthread -I.. -DTEXDIR="\"$(pwd)/../assets/textures\"" \
    facet_preview.cpp ../facetmesh.cpp ../vtex.cpp -o "$OUT"
mkdir -p "$1"
"$OUT" "$@"
