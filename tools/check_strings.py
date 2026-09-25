#!/usr/bin/env python3
"""Checks the string table rule (D26): every player-facing word is in
assets/text/<language>.txt, never written into code.

 1. Words in code. A string literal in the game layer (5) or main.cpp
    that reads like text -- capitals (the game's text is capitals: two in a
    row), or lowercase words with a space between -- fails, unless
      - it's a string-table key (lowercase words joined by dots), or
      - it's an argument to a debug-only call (ProfBootMark,
        OutputDebugStringA/W), or
      - its line ends with the marker   // D26: not player text   (with a
        short reason after it), or
      - it sits between   // D26: debug text   and   // D26: end   (the F3
        overlay, the performance report and other debug aids, which are
        for the developer and stay in English).
 2. Keys. Every key the code uses is defined in en.txt, and every key in
    en.txt is used (materials by their registry name, blocks.h).
 3. Translations. Every other language file parses, and its {0}-style
    slots match English's.

  python3 tools/check_strings.py                  check (exit 1 on failure)
  python3 tools/check_strings.py --pseudo         also write assets/text/test.txt,
      a pseudo-language: every text accented and a third longer, to see
      untranslated words (they stay plain) and layouts that don't stretch.
      Pick it with language=test in settings.cfg.
"""
import os, re, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
TEXT = os.path.join(ROOT, 'assets', 'text')
KEY = re.compile(r'^[a-z0-9_]+(\.[a-z0-9_]+)+$')
FILE = re.compile(r'\.(txt|dll|sav|cfg|png|vtex|cso)$')
DEBUG_CALL = re.compile(r'\b(ProfBootMark|OutputDebugStringA|OutputDebugStringW)\s*\(\s*$')
LIT = re.compile(r'(?:u8|L)?"((?:[^"\\]|\\.)*)"')


def parse(path):
    table, errors = {}, []
    with open(path, encoding='utf-8-sig') as f:
        for n, line in enumerate(f, 1):
            t = line.strip()
            if not t or t.startswith('#'):
                continue
            k, eq, v = t.partition('=')
            k = k.strip()
            if not eq or not re.match(r'^[a-z0-9_.]+$', k):
                errors.append('%s:%d: expected `key = text`' % (os.path.basename(path), n))
                continue
            table[k] = v.strip()
    return table, errors


def game_files():
    files = []
    for line in open(os.path.join(ROOT, 'tools', 'layers.txt')):
        parts = line.split()
        if len(parts) == 2 and not line.startswith('#') and parts[1] in ('5', '6'):
            files.append(parts[0])
    return files


def strip_comment(line):
    # The code part of a line: up to a // outside a string literal.
    out, i, q = [], 0, False
    while i < len(line):
        c = line[i]
        if q:
            out.append(c)
            if c == '\\' and i + 1 < len(line):
                out.append(line[i + 1]); i += 1
            elif c == '"':
                q = False
        else:
            if line.startswith('//', i):
                break
            if c == '"':
                q = True
            out.append(c)
        i += 1
    return ''.join(out)


def main():
    failures = []
    en, errs = parse(os.path.join(TEXT, 'en.txt'))
    failures += errs
    used = set()

    for name in game_files():
        path = os.path.join(ROOT, name)
        debug = False
        in_block_comment = False
        for n, raw in enumerate(open(path, encoding='utf-8-sig'), 1):
            if '// D26: debug text' in raw:
                debug = True
                continue
            if '// D26: end' in raw:
                debug = False
                continue
            s = raw.strip()
            if in_block_comment:
                if '*/' in s:
                    in_block_comment = False
                continue
            if s.startswith('/*'):
                in_block_comment = '*/' not in s
                continue
            if s.startswith('#include') or s.startswith('#pragma') or s.startswith('//'):
                continue
            code = strip_comment(raw)
            for m in LIT.finditer(code):
                text = m.group(1)
                if KEY.match(text) and not FILE.search(text):
                    used.add(text)
                    continue
                if debug or '// D26: not player text' in raw or DEBUG_CALL.search(code[:m.start()]):
                    continue
                if re.search(r'[A-Z]{2}', text) or re.search(r'[a-z]{2}.* .*[a-z]{2}', text):
                    failures.append('%s:%d: "%s" -- a player-facing word belongs in assets/text/en.txt '
                                    '(or mark the line // D26: not player text)' % (name, n, text))

    # Materials are named by their registry name (hud.cpp: "material." + name).
    for m in re.finditer(r'^\s*\{\s*"([a-z0-9_]+)",', open(os.path.join(ROOT, 'blocks.h')).read(), re.M):
        used.add('material.' + m.group(1))
    used.add('font')

    for k in sorted(used - set(en)):
        failures.append('en.txt: key "%s" is used in code but not defined' % k)
    for k in sorted(set(en) - used):
        failures.append('en.txt: key "%s" is defined but never used' % k)

    slots = lambda t: sorted(set(re.findall(r'\{\d+\}', t)))
    for f in sorted(os.listdir(TEXT)):
        if not f.endswith('.txt') or f == 'en.txt':
            continue
        table, errs = parse(os.path.join(TEXT, f))
        failures += errs
        for k, v in table.items():
            if k not in en:
                failures.append('%s: key "%s" is not in en.txt' % (f, k))
            elif k != 'font' and slots(v) != slots(en[k]):
                failures.append('%s: key "%s" has slots %s, English has %s' % (f, k, slots(v), slots(en[k])))

    if '--pseudo' in sys.argv:
        write_pseudo(en)

    for x in failures:
        print('FAIL ' + x)
    if failures:
        return 1
    print('ok   string table: %d keys, every one used and defined; no words in game code' % len(en))
    return 0


ACCENT = dict(zip('AEIOUCNY', 'ÂÉÎÖÜÇÑÝ'))


def write_pseudo(en):
    lines = ['# A pseudo-language for testing (tools/check_strings.py --pseudo):',
             '# every text accented and a third longer. Not a translation.',
             'font = ' + en.get('font', 'Consolas')]
    for k in sorted(en):
        if k == 'font':
            continue
        v = en[k]
        out, i = [], 0
        while i < len(v):  # keep {0} slots and \n intact
            m = re.match(r'\{\d+\}|\\n', v[i:])
            if m:
                out.append(m.group(0)); i += len(m.group(0)); continue
            out.append(ACCENT.get(v[i], v[i])); i += 1
        body = ''.join(out)
        pad = '~' * max(1, len(v) // 3)
        lines.append('%s = [%s%s]' % (k, body, pad))
    with open(os.path.join(TEXT, 'test.txt'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
    print('wrote assets/text/test.txt')


if __name__ == '__main__':
    sys.exit(main())
