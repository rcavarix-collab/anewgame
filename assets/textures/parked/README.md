# Parked textures

Art kept for later, but not loaded by the game: the loader reads only the
`.vtex` files directly in `assets/textures/`, so nothing in this folder
reaches the texture array (and no "unused texture" warning is raised).

- `industry.vtex`: the Voxistics pulse-logistics set (pipes, harvester,
  store, diffuser, plate). Parked in M0.6 when pulse left the game.
  Generator: `tools/industry_textures.py` (writes here).

To use a parked texture again, move its file back up a folder and give a
block a texture name from it.
