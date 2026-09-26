# Research: pixel art and palettes (for the 64-pixel materials, PLAN_TUESDAY step 9, and Grok's A tasks)

**Sources:**
- [s] Gerstner, DeCarlo, Alexa, Finkelstein, Gingold & Nealen, "Pixelated image abstraction", NPAR 2012 (expanded in *Computers & Graphics* 2013, "with integrated user constraints"). Its study included interviews with expert pixel artists.
- The full text couldn't be opened from here (the host is blocked); the summary is from its abstract. **To read:** the authors' copy on Princeton's graphics pages (gfx.cs.princeton.edu/pubs/Gerstner_2012_PIA).

## What the theory says (from the abstract; to be deepened when read)

- **Pixel art is a joint choice of shapes and palette:** the method solves for the mapping of features to pixels and a reduced palette together. The palette isn't picked after the drawing.
- **Artists judged the results against naive downsampling and colour reduction and preferred them,** so structure-aware reduction matters: features kept whole, not dissolved into noise.

## What it means for walkgrid (for the generators and Grok's briefs)

- **Design the palette with the marks.** Each material's 8–16 colours should be chosen for its marks (blade tips, chip edges, strata lines), not as a smooth ramp sampled later. Grok's A-series briefs already ask for a palette with roles.
- **Features whole, not noisy.** At 64 px a mark should be a readable cluster of a few texels, not speckle. Our value-noise generators should threshold into clusters (Voronoi cells, scattered shapes) rather than output per-texel noise.
- **Plan to check it.** The planned tools (P5 brightness and saturation, P2 sheets, P19 review page) let the owner judge each material's clusters and palette before it goes in.

## To read (legal copies)

- Gerstner et al. 2012, authors' copy (Princeton) and project page (cragl.cs.gmu.edu/pixelate).
