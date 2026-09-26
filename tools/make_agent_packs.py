#!/usr/bin/env python3
"""Makes the helper agent's copy-and-paste packs (docs/AGENT_TASKS.md).

One file per task in docs/agent_packs/<task>.md: the Brief, the task's
section and the excerpts of code and docs it needs, so the owner pastes a
single piece of text into the agent's chat and attaches nothing. The task
text and the Brief come from docs/AGENT_TASKS.md; the excerpts are cut
from the current files here, by marker (a function, a section, a range
between two lines), never by line number, so they survive edits.

Cost: none in the game; a tool run by Claude at milestones. Standard
library only (plus openpyxl for the scope sheet, as the repo already uses).

  python3 tools/make_agent_packs.py          write the packs
  python3 tools/make_agent_packs.py --check  exit 1 if an excerpt's marker
                                             is gone or a pack is too long
"""
import datetime, os, re, subprocess, sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
OUT = os.path.join(ROOT, 'docs', 'agent_packs')
TASKS = os.path.join(ROOT, 'docs', 'AGENT_TASKS.md')
# One chat message. Free chat plans cap a message's length (the exact cap
# isn't published), so a pack longer than this goes in numbered parts, each
# its own file. Lower it if a paste is refused or cut short.
PART_CHARS = 12000
PART_HEADER = 300   # room for a part's first line

class Missing(Exception):
    pass

def read(path):
    with open(os.path.join(ROOT, path), encoding='utf-8') as f:
        return f.read()

def whole(path):
    return read(path).rstrip('\n')

def between(path, start, end, include_end=True):
    """From the first line matching `start` to the next matching `end`."""
    lines = read(path).split('\n')
    for i, l in enumerate(lines):
        if re.search(start, l):
            for j in range(i + 1, len(lines)):
                if re.search(end, lines[j]):
                    return '\n'.join(lines[i:j + 1 if include_end else j]).rstrip('\n')
            raise Missing('%s: no end /%s/ after /%s/' % (path, end, start))
    raise Missing('%s: no line /%s/' % (path, start))

def line(path, pattern):
    """The one line matching `pattern`."""
    for l in read(path).split('\n'):
        if re.search(pattern, l):
            return l
    raise Missing('%s: no line /%s/' % (path, pattern))

def function(path, start, lead=0):
    """A C++ function (or struct): from the line matching `start` until its
    braces balance, with `lead` comment lines above it."""
    lines = read(path).split('\n')
    for i, l in enumerate(lines):
        if re.search(start, l):
            depth, opened = 0, False
            for j in range(i, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                opened |= '{' in lines[j]
                if opened and depth <= 0:
                    a = i
                    while lead and a > 0 and lines[a - 1].lstrip().startswith('//') and i - a < lead:
                        a -= 1
                    return '\n'.join(lines[a:j + 1])
            raise Missing('%s: unbalanced after /%s/' % (path, start))
    raise Missing('%s: no line /%s/' % (path, start))

def section(path, heading):
    """A Markdown section: its heading line (matching `heading`) to the next
    heading of the same or a higher level."""
    lines = read(path).split('\n')
    for i, l in enumerate(lines):
        m = re.match(r'(#+)\s', l)
        if m and re.search(heading, l):
            level = len(m.group(1))
            for j in range(i + 1, len(lines)):
                n = re.match(r'(#+)\s', lines[j])
                if n and len(n.group(1)) <= level:
                    return '\n'.join(lines[i:j]).rstrip('\n')
            return '\n'.join(lines[i:]).rstrip('\n')
    raise Missing('%s: no heading /%s/' % (path, heading))

def head(path, n):
    return '\n'.join(read(path).split('\n')[:n])

def decisions(ids):
    lines = read('docs/DECISIONS.md').split('\n')
    header = [l for l in lines if l.startswith('| # ')][:1] + ['|---|---|---|---|---|']
    rows = [l for l in lines if any(l.startswith('| %s |' % d) for d in ids)]
    if len(rows) != len(ids):
        raise Missing('docs/DECISIONS.md: rows %s' % ids)
    return '\n'.join(header + rows)

def scope_rows():
    import openpyxl
    ws = openpyxl.load_workbook(os.path.join(ROOT, 'docs', 'SCOPE_MOSCOW.xlsx'), read_only=True)['Scope']
    out = []
    for row in ws.iter_rows(values_only=True):
        if any(v is not None for v in row):
            out.append(' | '.join('' if v is None else str(v).replace('\n', ' ') for v in row))
    return '\n'.join(out)

def file_list():
    files = subprocess.run(['git', 'ls-files'], cwd=ROOT, capture_output=True, text=True, check=True).stdout.split('\n')
    return '\n'.join(f for f in files if f and not f.startswith(('reference/', 'incoming/', 'docs/agent_packs/')))

def lighting_lines():
    """The world shader's lighting: ShadowLit, and from the normal blend to
    the lit colour."""
    return between('render.cpp', r'float ShadowLit\(', r'^\s*"}\\n"') + '\n...\n' + \
        between('render.cpp', r'float farBlend = ', r'float3 col = albedo')

TEST_HEAD = lambda: ('tests/tests.cpp (top: includes and CHECK)', head('tests/tests.cpp', 60), 'cpp')
TEST_MAIN = lambda: ('tests/tests.cpp (main, at the bottom)', function('tests/tests.cpp', r'^int main\(\)'), 'cpp')

# Each task: its heading in AGENT_TASKS.md, and its excerpts as
# (label, text, fence language), made on demand so a missing marker names
# the task.
PACKS = {
    'R1': lambda: [
        ('docs/PROJECT_NOTES.md section 1', section('docs/PROJECT_NOTES.md', r'^## 1\. '), 'md'),
        ('DESIGN.md 4.8', section('DESIGN.md', r'^### 4\.8 '), 'md'),
        ('DESIGN.md 4.9', section('DESIGN.md', r'^### 4\.9 '), 'md'),
        ('DESIGN.md 23.3', section('DESIGN.md', r'^### 23\.3 '), 'md'),
        ('DESIGN.md 23.4', section('DESIGN.md', r'^### 23\.4 '), 'md'),
        ('DESIGN.md 23.5a', section('DESIGN.md', r'^### 23\.5a '), 'md'),
        ('sky.h: ComputeAtmosphere', function('sky.h', r'ComputeAtmosphere\(const SkyState', lead=12), 'cpp'),
        ('render.cpp: world shader lighting (a string of HLSL)', lighting_lines(), 'cpp'),
        ('docs/COST_LEDGER.md', whole('docs/COST_LEDGER.md'), 'md'),
    ],
    'R2': lambda: [
        ('docs/PROJECT_NOTES.md section 1', section('docs/PROJECT_NOTES.md', r'^## 1\. '), 'md'),
        ('DESIGN.md Part XIII', section('DESIGN.md', r'^## Part XIII '), 'md'),
        ('sky.h', whole('sky.h'), 'cpp'),
        ('docs/SOUND_PALETTE.md sections 1-3', between('docs/SOUND_PALETTE.md', r'^## 1\. ', r'^## 4\. ', include_end=False), 'md'),
    ],
    'R3': lambda: [
        ('facetmesh.h', whole('facetmesh.h'), 'cpp'),
        ('facetmesh.cpp: CornerPos', function('facetmesh.cpp', r'^bool CornerPos\(', lead=20), 'cpp'),
        ('terrain.h', whole('terrain.h'), 'cpp'),
        ('docs/DECISIONS.md rows', decisions(['D35', 'D39', 'D55', 'D62']), 'md'),
    ],
    'R4': lambda: [
        ('docs/PROJECT_NOTES.md section 1', section('docs/PROJECT_NOTES.md', r'^## 1\. '), 'md'),
        ('DESIGN.md 23.1', section('DESIGN.md', r'^### 23\.1 '), 'md'),
        ('DESIGN.md 23.5a', section('DESIGN.md', r'^### 23\.5a '), 'md'),
        ('assets/textures/TEXTURE_BRIEF.md', whole('assets/textures/TEXTURE_BRIEF.md'), 'md'),
    ],
    'R5': lambda: [
        ('settings.h', whole('settings.h'), 'cpp'),
        ('DESIGN.md 4.8', section('DESIGN.md', r'^### 4\.8 '), 'md'),
        ('DESIGN.md 23.6', section('DESIGN.md', r'^### 23\.6 '), 'md'),
        ('docs/COST_LEDGER.md', whole('docs/COST_LEDGER.md'), 'md'),
        ('docs/FOUNDATIONS.md section 4', section('docs/FOUNDATIONS.md', r'^## 4\. '), 'md'),
    ],
    'R6': lambda: [
        ('terrain.h', whole('terrain.h'), 'cpp'),
        ('terrain.cpp', whole('terrain.cpp'), 'cpp'),
        ('DESIGN.md 23.2', section('DESIGN.md', r'^### 23\.2 '), 'md'),
        ('DESIGN.md 23.7', section('DESIGN.md', r'^### 23\.7 '), 'md'),
        ('docs/FOUNDATIONS.md section 3', section('docs/FOUNDATIONS.md', r'^## 3\. '), 'md'),
        ('docs/FOUNDATIONS.md section 5', section('docs/FOUNDATIONS.md', r'^## 5\. '), 'md'),
    ],
    'R7': lambda: [
        ('docs/GOALS.md', whole('docs/GOALS.md'), 'md'),
        ('docs/ROADMAP.md', whole('docs/ROADMAP.md'), 'md'),
        ('docs/PROJECT_NOTES.md section 9', section('docs/PROJECT_NOTES.md', r'^## 9\. '), 'md'),
        ('docs/SCOPE_MOSCOW.xlsx, sheet Scope (one row per line)', scope_rows(), 'text'),
    ],
    'T1': lambda: [
        ('assets/text/en.txt', whole('assets/text/en.txt'), 'text'),
        ('strtable.h', whole('strtable.h'), 'cpp'),
    ],
    'T2': lambda: [
        ('assets/text/en.txt', whole('assets/text/en.txt'), 'text'),
        ('CLAUDE.md: the minimal text rule', line('CLAUDE.md', r'Minimal text, any language'), 'md'),
    ],
    'X1': lambda: [
        ('sky.h', whole('sky.h'), 'cpp'),
        TEST_HEAD(), TEST_MAIN(),
    ],
    'X2': lambda: [
        ('strtable.h', whole('strtable.h'), 'cpp'),
        ('strtable.cpp', whole('strtable.cpp'), 'cpp'),
        TEST_HEAD(), TEST_MAIN(),
    ],
    'X3': lambda: [
        ('facetmesh.h', whole('facetmesh.h'), 'cpp'),
        ('tests/tests.cpp: TestHiddenChunks', function('tests/tests.cpp', r'^static void TestHiddenChunks\(', lead=4), 'cpp'),
        TEST_HEAD(), TEST_MAIN(),
    ],
    'P1': lambda: [
        ('worldfile.h', whole('worldfile.h'), 'cpp'),
        ('worldfile.cpp', whole('worldfile.cpp'), 'cpp'),
        ('savegame.cpp', whole('savegame.cpp'), 'cpp'),
    ],
    'P2': lambda: [
        ('tools/check_strings.py (top, for style)', head('tools/check_strings.py', 80), 'python'),
        ('the repository\'s files (git ls-files, without reference/ and incoming/)', file_list(), 'text'),
    ],
    'F1': lambda: [
        ('hud.cpp: the F3 debug block', between('hud.cpp', r'// D26: debug text \(the F3', r'// D26: end'), 'cpp'),
        ('profiler.h', whole('profiler.h'), 'cpp'),
        ('docs/FOUNDATIONS.md section 4', section('docs/FOUNDATIONS.md', r'^## 4\. '), 'md'),
        ('docs/COST_LEDGER.md', whole('docs/COST_LEDGER.md'), 'md'),
    ],
    'F2': lambda: [
        ('hud.cpp: the drawRowButton helper', function('hud.cpp', r'auto drawRowButton = ', lead=6), 'cpp'),
        ('hud.cpp: the Keybindings screen', between('hud.cpp', r'g_menuScreen == MenuScreen::Keybindings\)', r'^    }$'), 'cpp'),
        ('game_internal.h: the Keybindings part', between('game_internal.h', r'^// Keybindings: every action', r'g_rebindingAction'), 'cpp'),
        ('settings.h', whole('settings.h'), 'cpp'),
    ],
    'V1': lambda: [('sky.h', whole('sky.h'), 'cpp')],
    'V2': lambda: [
        ('render.cpp: shadow state and ShadowFade', between('render.cpp', r'^static Mat4 g_lightViewProj = ', r'^static uint32_t g_buildMeshVersion'), 'cpp'),
        ('render.cpp: UpdateShadowMap', function('render.cpp', r'^static void UpdateShadowMap\('), 'cpp'),
        ('render.cpp: world shader, ShadowLit and its use (a string of HLSL)', between('render.cpp', r'float ShadowLit\(', r'^\s*"}\\n"') + '\n...\n' +
            between('render.cpp', r'float shadow = 1\.0f;', r'sunLit \*= shadow;'), 'cpp'),
        ('render.cpp: where the fade is passed to the shader', between('render.cpp', r'cb\.lightViewProj = g_lightViewProj;', r'cb\.params\[3\] = '), 'cpp'),
    ],
    'V3': lambda: [
        ('terrain.h', whole('terrain.h'), 'cpp'),
        ('terrain.cpp', whole('terrain.cpp'), 'cpp'),
    ],
    'V4': lambda: [
        ('savegame.cpp', whole('savegame.cpp'), 'cpp'),
        ('gamefiles.h', whole('gamefiles.h'), 'cpp'),
        ('gamefiles.cpp', whole('gamefiles.cpp'), 'cpp'),
    ],
}

def brief(text):
    b = text[text.index('## The Brief'):]
    b = b[b.index('\n') + 1:b.index('\n---')]
    return '\n'.join(l[2:] if l.startswith('> ') else l.lstrip('>') for l in b.strip('\n').split('\n'))

def task_text(text, tid):
    lines = text.split('\n')
    for i, l in enumerate(lines):
        if l.startswith('**%s. ' % tid):
            out = [l]
            for m in lines[i + 1:]:
                if m.startswith(('**', '#', '---')) or (m.strip() == '' and tid.startswith('V')):
                    break
                out.append(m)
            # The group's shared notes (research notes, test style, reviews)
            # sit between the group heading and its first task.
            j = i - 1
            while j >= 0 and not lines[j].startswith('### '):
                j -= 1
            first = next(k for k in range(j + 1, len(lines)) if lines[k].startswith('**'))
            shared = '\n'.join(lines[j + 1:first]).strip()
            return ('\n'.join(out).strip(), shared)
    raise Missing('docs/AGENT_TASKS.md: no task %s' % tid)

def fence(body):
    n = 3
    while '`' * n in body:
        n += 1
    return '`' * n

def blocks(tid, text, date):
    """The pack as its opening (Brief and task), its excerpts as
    (label, body, fence language), and its closing line."""
    task, shared = task_text(text, tid)
    first = ['# walkgrid helper task %s (pack made %s)' % (tid, date), '',
             'This is: the Brief (how to work and hand back), then your task, then the excerpts of the project you need. You have nothing else: if the task needs something not here, say so in the report.',
             '', '## The Brief', '', brief(text), '', '## Your task', '']
    if shared:
        first += [shared, '']
    first += [task, '', '## Excerpts', '',
              'These are cut from the current files. "..." marks a gap. Copy Find text for EDITS.md only from here.', '']
    last = '\n'.join(['## Now', '', 'Do task %s as the Brief says: `REPORT.md` first, then each file under its `=== FILE:` line, and `END OF DELIVERY` at the very end.' % tid, ''])
    return '\n'.join(first), PACKS[tid](), last

def make(tid, text, date):
    """The pack's parts, each one paste of at most PART_CHARS. An excerpt
    that doesn't fit the rest of a part is cut at a line end, and its
    pieces are labelled so they can be joined back."""
    first, excerpts, last = blocks(tid, text, date)
    room = PART_CHARS - PART_HEADER
    parts, cur = [], first + '\n'
    for label, body, lang in excerpts:
        f = fence(body)
        lines, piece = body.split('\n'), 1
        while lines:
            wrap = len(label) + 2 * len(f) + len(lang) + 80   # heading, fences, piece label
            take, size = 0, 0
            while take < len(lines) and len(cur) + wrap + size + len(lines[take]) + 1 <= room:
                size += len(lines[take]) + 1
                take += 1
            if take < len(lines) and (take == 0 or take < 20): # too little room left: start a new part
                if cur.strip() == '':
                    raise Missing('a line longer than a part in %s' % label)
                parts.append(cur); cur = ''
                continue
            whole_fits = take == len(lines) and piece == 1
            name = label if whole_fits else '%s (piece %d; the pieces join line to line)' % (label, piece)
            cur += '\n'.join(['### %s' % name, '', f + lang] + lines[:take] + [f, '']) + '\n'
            lines, piece = lines[take:], piece + 1
    if len(cur) + len(last) > room:
        parts.append(cur); cur = ''
    parts.append(cur + last)
    n = len(parts)
    if n == 1:
        return parts
    out = []
    for k, body in enumerate(parts, 1):
        if k < n:
            head = ('**Part %d of %d** of walkgrid helper task %s. More parts follow: don\'t start the task yet. '
                    'Reply only `got part %d of %d`.' % (k, n, tid, k, n))
        else:
            head = ('**Part %d of %d, the last** of walkgrid helper task %s. You now have everything (read it together with the '
                    'earlier parts): do the task.' % (k, n, tid))
        out.append(head + '\n\n' + body)
    return out

def main():
    check = '--check' in sys.argv
    text = read('docs/AGENT_TASKS.md')
    date = datetime.date.today().isoformat()
    problems, made = [], []
    for tid in PACKS:
        try:
            parts = make(tid, text, date)
        except Missing as e:
            problems.append('%s: %s' % (tid, e))
            continue
        names = [tid + '.md'] if len(parts) == 1 else ['%s-%d.md' % (tid, k) for k in range(1, len(parts) + 1)]
        made.append((tid, names, [len(p) for p in parts]))
        for name, p in zip(names, parts):
            if len(p) > PART_CHARS:
                problems.append('%s: %d characters, over %d for one paste' % (name, len(p), PART_CHARS))
    if not check:
        os.makedirs(OUT, exist_ok=True)
        for old in os.listdir(OUT): # packs from an earlier run may have had more parts
            if old.endswith('.md'):
                os.remove(os.path.join(OUT, old))
        for tid, names, _ in made:
            for name, p in zip(names, make(tid, text, date)):
                with open(os.path.join(OUT, name), 'w', encoding='utf-8', newline='\n') as f:
                    f.write(p)
        rows = ['# Packs for the helper agent', '',
                'Made %s by `tools/make_agent_packs.py` from `docs/AGENT_TASKS.md` and the code; don\'t edit these by hand.' % date, '',
                'Each paste is at most %d characters. A task with several parts: paste part 1 into a fresh chat, wait for "got part 1", paste part 2, and so on; the agent starts at the last part. See `docs/AGENT_TASKS.md`, "For the owner".' % PART_CHARS, '',
                '| Task | What | Paste these, in order |', '|---|---|---|']
        for tid, names, _ in made:
            title = task_text(text, tid)[0].split('\n')[0].strip('*')
            title = re.sub(r'\*\*.*', '', title).strip().rstrip('.')
            rows.append('| %s | %s | %s |' % (tid, title[len(tid) + 2:], ', '.join('[%s](%s)' % (n[:-3], n) for n in names)))
        with open(os.path.join(OUT, 'README.md'), 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(rows) + '\n')
    for tid, names, sizes in made:
        print('%-3s %d part(s): %s' % (tid, len(names), ' '.join(str(n) for n in sizes)))
    for p in problems:
        print('PROBLEM', p)
    return 1 if problems else 0

if __name__ == '__main__':
    sys.exit(main())
