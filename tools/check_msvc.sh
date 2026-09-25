#!/bin/sh
# Catches what the owner's Visual Studio build (SDL checks on) rejects but a
# MinGW/GCC build doesn't: the CRT functions MSVC deprecates (C4996, an
# error under /sdl). Scans every source the project compiles, plus headers.
# Usage: tools/check_msvc.sh   (from anywhere; exit 1 on a finding)
cd "$(dirname "$0")/.."
SRCS=$(grep -o 'ClCompile Include="[^"]*"' $(ls *.vcxproj | head -n 1) | sed 's/.*="//;s/"//')
BANNED='getenv|_wgetenv|fopen|_wfopen|freopen|_wfreopen|sprintf|vsprintf|strcpy|wcscpy|strcat|wcscat|strncpy|strncat|sscanf|swscanf|fscanf|scanf|strtok|wcstok|localtime|gmtime|ctime|asctime|mbstowcs|wcstombs|_itoa|itoa|_snprintf|_open|_wopen|strerror|tmpnam|strdup|fileno|unlink|getcwd|chdir|stricmp|strnicmp'
HITS=$(grep -nE "(^|[^_A-Za-z0-9])($BANNED)[[:space:]]*\(" $SRCS *.h 2>/dev/null | grep -vE "(snprintf|vsnprintf)[[:space:]]*\(" )
if [ -n "$HITS" ]; then
    echo "MSVC /sdl would reject these (C4996 deprecated CRT calls):"
    echo "$HITS"
    exit 1
fi
# Words <windows.h> defines as macros (rpcndr.h: small is char; minwindef.h:
# near, far; objbase.h: interface), so as names they break the MSVC build
# but not MinGW's, which doesn't pull those headers in the same way. Code
# only: comments and string literals are skipped.
MACROS=$(python3 - $SRCS *.h <<'PY'
import re, sys
bad = re.compile(r'\b(small|near|far|hyper|interface|pascal|cdecl)\b')
for path in sys.argv[1:]:
    try: lines = open(path, encoding='utf-8-sig', errors='replace').read().split('\n')
    except OSError: continue
    block = False
    for n, line in enumerate(lines, 1):
        code, i, q = '', 0, False
        while i < len(line):
            c = line[i]
            if block:
                if line.startswith('*/', i): block = False; i += 2; continue
                i += 1; continue
            if q:
                if c == '\\': i += 2; continue
                if c == '"': q = False
                i += 1; continue
            if line.startswith('//', i): break
            if line.startswith('/*', i): block = True; i += 2; continue
            if c == '"': q = True; i += 1; continue
            if c == "'" and i + 2 < len(line): j = line.find("'", i + 1); i = (j + 1) if j > 0 else i + 1; continue
            code += c; i += 1
        if bad.search(code): print('%s:%d: %s' % (path, n, line.strip()))
PY
)
if [ -n "$MACROS" ]; then
    echo "Names <windows.h> defines as macros (MSVC fails on these; rename them):"
    echo "$MACROS"
    exit 1
fi
echo "ok   no MSVC-deprecated CRT calls, no Windows macro names"
