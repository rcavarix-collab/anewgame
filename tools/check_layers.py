#!/usr/bin/env python3
"""Enforces the layer rule (docs/FOUNDATIONS.md 2): a source file may include
only project headers from its own layer or a lower one, so the engine never
names the game. Layers come from tools/layers.txt; every file the project
compiles, and every project header, must be listed there.

  python3 tools/check_layers.py     exit 1 on any violation or unlisted file
"""
import os, re, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
SEEDS = {'Prismative.cpp', 'drillder.cpp', 'LG2.cpp', 'cc_2_2_2.cpp'}  # reference only, never compiled

def main():
    layers = {}
    with open(os.path.join(ROOT, 'tools', 'layers.txt')) as f:
        for line in f:
            line = line.split('#', 1)[0].split()
            if len(line) == 2:
                layers[line[0]] = int(line[1])
    sources = sorted(n for n in os.listdir(ROOT) if n.endswith(('.cpp', '.h')) and n not in SEEDS)
    problems = []
    for name in sources:
        if name not in layers:
            problems.append(f'{name}: not in tools/layers.txt (give it a layer)')
            continue
        with open(os.path.join(ROOT, name), encoding='utf-8', errors='replace') as f:
            for n, line in enumerate(f, 1):
                m = re.match(r'\s*#\s*include\s+"([^"]+)"', line)
                if not m or m.group(1) not in layers:
                    continue
                inc = m.group(1)
                if layers[inc] > layers[name]:
                    problems.append(f'{name}:{n}: layer {layers[name]} includes {inc} (layer {layers[inc]})')
    if problems:
        print('Layer rule broken (docs/FOUNDATIONS.md 2):')
        for p in problems:
            print('  ' + p)
        return 1
    print('ok   layer rule holds')
    return 0

if __name__ == '__main__':
    sys.exit(main())
