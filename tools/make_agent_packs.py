#!/usr/bin/env python3
"""Makes the helper agent's copy-and-paste packs (docs/AGENT_TASKS.md).

One file per task in docs/agent_packs/<task>.md: the Brief and the task,
with the task's name at the top and the bottom, so the owner pastes a
single self-contained piece of text into a fresh chat and attaches
nothing. The agent gets no whole files (owner, 2026-09-26): each task in
AGENT_TASKS.md carries the facts it needs in plain words, plus a few lines
where a format matters. The only file carried is the player's text
(assets/text/en.txt) for T1 and T2, which work on those words.

Cost: none in the game; a tool Claude reruns whenever AGENT_TASKS.md
changes. Standard library only.

  python3 tools/make_agent_packs.py          write the packs
  python3 tools/make_agent_packs.py --check  exit 1 if a pack is too long
"""
import datetime, os, re, sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
OUT = os.path.join(ROOT, 'docs', 'agent_packs')
# One chat message. Free chat plans cap a message's length (the exact cap
# isn't published); lower it if a paste is refused or cut short.
MAX_CHARS = 12000
# Tasks that work on the player's own words carry them.
CARRY_TEXT = ('T1', 'T2')

def read(path):
    with open(os.path.join(ROOT, path), encoding='utf-8') as f:
        return f.read()

def brief(text):
    b = text[text.index('## The Brief'):]
    b = b[b.index('\n') + 1:b.index('\n---')]
    return '\n'.join(l[2:] if l.startswith('> ') else l.lstrip('>') for l in b.strip('\n').split('\n'))

def task_ids(text):
    return re.findall(r'^\*\*([A-Z]\d+)\. ', text, re.M)

def task_text(text, tid):
    """The task's section, and its group's shared notes (between the group
    heading and the group's first task)."""
    lines = text.split('\n')
    i = next(k for k, l in enumerate(lines) if l.startswith('**%s. ' % tid))
    out = [lines[i]]
    for m in lines[i + 1:]:
        if m.startswith(('**', '#', '---')):
            break
        out.append(m)
    j = i - 1
    while not lines[j].startswith('### '):
        j -= 1
    first = next(k for k in range(j + 1, len(lines)) if lines[k].startswith('**'))
    return '\n'.join(out).strip(), '\n'.join(lines[j + 1:first]).strip()

def title(task):
    t = task.split('\n')[0]
    return re.sub(r'^\*\*[A-Z]\d+\. |\*\*.*$', '', t).strip().rstrip('.')

def make(tid, text, date):
    task, shared = task_text(text, tid)
    name = '%s, %s' % (tid, title(task))
    # The agent may remember other tasks from earlier chats (its memory
    # once carried task R2's inputs into an R1 chat): say which one this is.
    parts = ['# walkgrid helper task %s' % name, '',
             'Everything for this task is in this one message (made %s). It is task %s only: ignore any other walkgrid task you may remember from earlier chats.' % (date, tid), '',
             brief(text), '', '## Your task: %s' % tid, '']
    if shared:
        parts += [shared, '']
    parts += [task, '']
    if tid in CARRY_TEXT:
        body = read('assets/text/en.txt').rstrip('\n')
        parts += ['## en.txt', '', '```text', body, '```', '']
    parts += ['## End of task %s' % tid, '',
              'Do task %s now: the report under `=== FILE: REPORT.md ===` first, then each file under its `=== FILE:` line, and `END OF DELIVERY` at the very end.' % tid, '']
    return '\n'.join(parts)

def main():
    check = '--check' in sys.argv
    text = read('docs/AGENT_TASKS.md')
    date = datetime.date.today().isoformat()
    packs = [(tid, make(tid, text, date)) for tid in task_ids(text)]
    problems = ['%s: %d characters, over %d for one paste' % (t, len(p), MAX_CHARS) for t, p in packs if len(p) > MAX_CHARS]
    if not check:
        os.makedirs(OUT, exist_ok=True)
        for old in os.listdir(OUT): # packs from an earlier run, including ones in parts
            if old.endswith('.md'):
                os.remove(os.path.join(OUT, old))
        for tid, p in packs:
            with open(os.path.join(OUT, tid + '.md'), 'w', encoding='utf-8', newline='\n') as f:
                f.write(p)
        rows = ['# Packs for the helper agent', '',
                'Made %s by `tools/make_agent_packs.py` from `docs/AGENT_TASKS.md`; don\'t edit these by hand.' % date, '',
                'One paste per task, into a **new private chat** each time: open the file, press **Copy raw file**, paste, send. See `docs/AGENT_TASKS.md`, "For the owner".', '',
                '| Task | What |', '|---|---|']
        for tid, _ in packs:
            rows.append('| [%s](%s.md) | %s |' % (tid, tid, title(task_text(text, tid)[0])))
        with open(os.path.join(OUT, 'README.md'), 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(rows) + '\n')
    for tid, p in packs:
        print('%-3s %6d' % (tid, len(p)))
    for p in problems:
        print('PROBLEM', p)
    return 1 if problems else 0

if __name__ == '__main__':
    sys.exit(main())
