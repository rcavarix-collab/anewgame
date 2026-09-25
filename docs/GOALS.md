# walkgrid goals

What we're aiming for, written so each goal can be checked. "Set by" says who set it; only the owner sets or changes an owner goal.

## The game

| # | Goal | How we'll know | Set by |
|---|---|---|---|
| G1 | A world that looks like nothing else: faceted ground over a grid, not cubes (Minecraft) and not blobs (Astroneer) | Screenshots read as walkgrid at a glance; nobody's first word is "Minecraft" | Owner |
| G2 | Ground that looks natural where materials meet | Borders read as ragged and natural, never as a triangle quilt (checklist T3) | Owner |
| G3 | Music and reactive sound that fit together and never fight | Effects stay in key and in time; nothing muddies the music (T7, T9) | Owner |
| G4 | Lots of action without lag, when action arrives | The action budgets hold on the reference machine | Owner |
| G5 | You can shape the world: place and remove ground | T8 | Owner |
| G6 | Playable in other languages | Every player-facing word comes from the string table; a test language shows no English left and no missing characters (D26) | Owner |
| G7 | All sound is synthesized | No audio files anywhere in the game (D25) | Owner |

## The engine

| # | Goal | How we'll know | Set by |
|---|---|---|---|
| E1 | Never below 60 fps on the reference machine, and smooth above it | T12, T13; the Ctrl+F3 report | Owner |
| E2 | Draw only what can be seen; load ahead what may be seen soon | Chunks drawn stays close to chunks in view; walking into new ground never hitches (worst frame ≤ 16.7 ms) | Owner |
| E3 | Cost grows with what's on screen or changing, never with world size or play time | The same scene costs the same after an hour of play, and after walking far | Owner |
| E4 | Runs on less than the reference machine | The floor (8 GB RAM, 2 GB graphics card) holds at lower settings | Owner |
| E5 | Fast to start | The boot timeline stays under the M0 baseline (to be measured) | Carried from Voxistics |
| E6 | Private: nothing leaves the machine, and nothing about it can be taken from the game | No network code, ever; no online play (D23); only harmless reads of limits, never kept or shown (D21) | Owner |

## How we work

| # | Goal | How we'll know | Set by |
|---|---|---|---|
| W1 | Nothing working gets rewritten without a reason | Every rewrite has an approved card (SOP) | Owner |
| W2 | Nothing gets in without earning its place | Every addition has a feature card and a ledger line (SOP) | Owner |
| W3 | The docs always match the code | A mismatch found is fixed in the same step | Owner |
| W4 | Every report says what wasn't verified | No report presents test counts as proof the game works | Owner |
