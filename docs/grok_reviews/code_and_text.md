# Grok T1, X1, X2, P1: appraisal

Delivered 2026-09-26. Each was **run**, not just read: the save reader on a real save written by the game's own encoder; the tests compiled against the real `sky.h` and `strtable.cpp`; the translations checked key by key against `en.txt`. Everything as delivered is kept in `incoming/grok/` (outside the build). Nothing is wired in until the go-ahead.

## P1 `dump_save.py` (save file reader): good

**Checks run:**
- A real save (317 bytes: two edited chunks, state, data, updates, the day tag): every field came out exactly as written, including seed, time, day count and cell counts. Checksum OK.
- Truncated file: clear message, no stack trace.
- One flipped byte: checksum MISMATCH reported.
- A non-save file: bad magic reported.
- Empty file: "too short".

**Notes:**
- It needs Python 3.9 or later (its type hints).
- It reads the file only.
- **Keep:** it goes to `tools/dump_save.py` after the go-ahead, with the usual header comment.

## X2 `TestStringsEdges` (text table tests): good, one bug

**Result:** 20 of 22 checks pass. The two failures are the test's own bug: in C++ `"ab\x80cd"` is read as one escape `\x80cd`, so the letters "cd" never reach the string. The fix is `"ab\x80" "cd"`, after which it would pass.

**Keep:** fixed, into `tests/tests.cpp` after the go-ahead.

## X1 `TestSkyEdges` (sky tests): fair

**Result:** 233 of 239 checks pass.
- **Jet stream:** it compares two moments one second apart with a tolerance of 0.0001 radians. The wind legitimately turns more than that in a second (measured differences up to about 0.0001 and above), so it fails without anything being wrong. A real continuity test compares moments a hair either side of midnight.
- **Eclipse search:** the task (Claude's wording) said to search only near noon. In 400 days the sky has one total solar eclipse, on day 99 in the morning, so a noon-only search finds none. The instruction was at fault, not Grok.

**Keep:** the disc-overlap and moon-phase checks. Drop the eclipse part: `TestSky` already checks that a total eclipse darkens the day. Fix the jet-stream tolerance.

## T1 translations (German, Mexican Spanish, French): good mechanically, needs a native read

**Mechanical checks:**
- All three have all 126 keys, in English's order.
- Every `{0}` slot matches.
- No character falls outside the font's fixed Latin-1 set.
- Nothing is left in English except `FPS` and `PROFILER`, which is fine.

**Language, by Claude (not a native speaker):**
- **German:**
  - `HAUTLAUTSTÄRKE` is a typo for `HAUPTLAUTSTÄRKE` ("skin volume" instead of "main volume").
  - `SPIELE WIE GEWOHNT` should be `SPIEL WIE GEWOHNT`.
  - "Feature Level … Support" is anglicised; acceptable for a technical message.
- **Spanish:**
  - `MARGAS` (plural) should be `MARGA`.
  - `RESTAURAR PREDETERMINADO` is long for a button; `RESTAURAR` alone would do.
- **French:**
  - `CLIQUEZ UNE LIGNE` should be `CLIQUEZ SUR UNE LIGNE`.
  - `QUITTER VERS TITRE` is awkward; `MENU PRINCIPAL` is better.
  - `AO ESPACE ÉCRAN` is jargon, as in English (T2).
- **All three** translate today's English, including the texts T2 is about to change (`DEG`, `30 S`, the SSAO name). Those lines need redoing after T2's edits.

**Keep:** into `assets/text/` after the go-ahead, with the fixes above and T2's changes carried through. A native speaker's read would be better still, especially for Spanish.
