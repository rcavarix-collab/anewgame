# Meadow grass

## Design target

The most common ground material: lively, cool-green meadow grass with short, angled blades. It should feel varied and natural while remaining crisp, readable pixel art at 64 × 64 pixels per block.

The faceted terrain is the main visual feature. Grass marks should follow the projected texture without competing with the angular ground planes.

Use compact blade clusters, small colour shifts, and irregular gaps to create a sense of living ground. Avoid evenly spaced blades, repeated motifs, and large areas of uniform green.

Grass should remain recognisably green under dawn, noon, dusk, and moonlight. Keep the palette cool and moderately saturated, with enough separation between base, shadow, and highlight values. Never rely on fluorescent yellow-green for brightness.

## Palette

Use these 12 colours as the complete meadow-grass palette. Hex values are sRGB.

| Hex | Role | Use |
|---|---|---|
| #263F35 | Deep shadow | Small gaps between dense blade clusters; darkest grass recesses |
| #304C3D | Shadow | Shaded blade bases and sparse dark patches |
| #3B5A45 | Cool dark green | Main shadowed ground variation |
| #466A4C | Base | Primary meadow ground colour |
| #507650 | Base light | Gentle variation across the ground |
| #5B8154 | Mid-green | Common blade colour |
| #648B59 | Blade highlight | Light-facing blade sections |
| #709362 | Soft highlight | Occasional brighter blades and small tufts |
| #7B9A69 | Pale green accent | Rare blade tips and tiny sunlit marks |
| #394F45 | Cool muted green | Blue-green shade variation without turning blue |
| #526653 | Muted olive-green | Subtle dry notes within the meadow |
| #68765A | Neutral moss-green | Rare mixed-ground flecks and subdued variation |

The base should occupy most of the texture. Dark colours should define gaps and depth, not form a continuous black-looking network. Highlights must remain subordinate to the mid-greens.

Use #7B9A69 sparingly. It is a pale green accent, not a target for the overall grass colour. Do not introduce pure white, fluorescent green, or extra colours outside this palette.

## Marks

All sizes refer to the 64 × 64 pixel texture for one block.

### 1. Ground colour patches

- Shape: Soft-edged, irregular patches produced by low-frequency value noise.
- Size: Approximately 8–24 pixels across.
- Count: 5–12 broad patches per block, with overlapping boundaries.
- Spread: Cover the full block; avoid a central patch or a repeating corner pattern.
- Colour: Mostly #466A4C and #507650, with subdued areas of #3B5A45 and #5B8154.
- Edge: Use stepped pixel boundaries, not blurred edges.

These patches provide broad variation without looking like separate painted spots. Their boundaries should be irregular and should not form stripes or rings.

### 2. Short grass blades

- Shape: Narrow, angled strokes, usually 1–2 pixels wide, with a tapered or stepped tip.
- Length: 3–7 pixels.
- Count: Approximately 55–90 blades per block.
- Spread: Clustered irregularly across the surface, with small open gaps between clusters.
- Orientation: Prefer diagonals, with a varied mixture of rising-left and rising-right blades. Use occasional near-vertical blades, but avoid a uniform direction.
- Colour: Primarily #5B8154 and #648B59; use #507650 for shaded blades.
- Edge: Crisp, hard-edged pixels.

A blade should read as a small angled mark, not a long line. Vary lengths and angles within each cluster. Avoid making every blade a single straight, identical stroke.

### 3. Dense blade clusters

- Shape: Small, irregular groups of 3–7 blades, with overlapping bases and separated tips.
- Footprint: 5–10 pixels wide and 5–9 pixels tall.
- Count: Approximately 8–14 clusters per block.
- Spread: Distribute unevenly, with some nearby clusters and some larger gaps.
- Colour: Mix #466A4C, #5B8154, #648B59, and occasional #709362.
- Structure: Cluster blades should diverge from a loose base rather than radiate symmetrically.

Clusters give the meadow its distinctive mark language. They should be visible at 64 pixels but remain small enough that the surface still reads as grass-covered ground.

### 4. Tiny ground gaps

- Shape: Irregular 1–3 pixel dark gaps, sometimes joined into a small notch.
- Count: Approximately 18–35 per block.
- Spread: Mostly around blade bases and between dense clusters.
- Colour: #263F35 and #304C3D.
- Limit: Keep gaps isolated. Do not connect them into a continuous web.

These gaps separate blades and give clusters definition. They must not turn the texture into a dark, cracked surface.

### 5. Rare pale tips

- Shape: Short 1–2 pixel marks, occasionally attached to the end of a blade.
- Count: Approximately 4–10 per block.
- Spread: Irregularly distributed, with no regular spacing.
- Colour: #709362 and, very rarely, #7B9A69.
- Limit: Keep the palest colour to a small fraction of the texture.

These accents should suggest occasional light catching the grass, not flowers or glitter.

### 6. Subtle mixed-green flecks

- Shape: Small irregular patches, 2–5 pixels across.
- Count: Approximately 6–14 per block.
- Spread: Scattered through the ground, preferentially within broad colour patches rather than on a regular grid.
- Colour: #394F45, #526653, or #68765A.
- Limit: Keep these colours secondary to the main green palette.

Use these flecks to break up repeated ground colour. They should not read as individual pebbles, leaves, or a second material.

### Overall density

Keep roughly 60–75% of the texture visually occupied by the ground base and broad colour variation, with blades and clusters providing the main fine detail. Dark gaps and pale accents should remain subordinate.

The listed mark counts are starting ranges for a generator, not a requirement to place every mark at maximum density simultaneously. Avoid overcrowding.

## Top vs side

### Top texture

Use the full meadow-grass mark language:

- Broad green ground variation.
- Short angled blades.
- Irregular blade clusters.
- Tiny dark gaps and rare pale tips.

The top texture is the primary representation of meadow grass. It should read clearly from above and at oblique viewing angles.

### Side texture

Use a quieter version of the same palette and marks:

- Retain broad green variation.
- Reduce blade density substantially.
- Use short, sparse blade ends near the upper edge of the visible face.
- Keep most of the side surface as compact green ground colour.
- Avoid long vertical blades or repeated horizontal bands.

The side should suggest grass-covered earth without looking like a vertical wall covered in long strands.

Use the same palette for both textures. Do not introduce a separate, much darker side palette.

## Height

Use the existing 36-level height-map range, where lower values represent recessed texels and higher values represent raised features.

Suggested levels:

- **Level 4–9:** Deep gaps between clusters and the darkest ground recesses.
- **Level 8–13:** Most of the ground base and broad colour variation.
- **Level 12–17:** Ordinary blade marks, with only modest elevation above the ground.
- **Level 17–21:** Selected blade tips and small cluster accents.
- **Level 22–25:** Rare short tuft tips, restricted to a few pixels.

Keep most pixels between levels 8 and 17. Use levels 22–25 sparingly.

The height map should describe compact grass relief, not tall grass standing above the terrain. Individual blades may have a small height increase, but their entire length should not become a raised ridge.

Do not assign high values to broad colour patches. Colour variation alone should not create raised terrain features.

Avoid long, thin high-height streaks. In particular, do not give an entire blade a uniformly high height value. Keep the highest values confined to small tip pixels or compact tuft tips. This prevents grass from producing conspicuous streaks when it blends with other materials.

At material boundaries, the few raised tips may win over lower neighbouring texels. Their small footprint should make this read as occasional grass tufts rather than a jagged, continuous border.

## Variation

Neighbouring blocks must not look like copies or reveal a 64 × 64 repeating grid.

Use deterministic variation derived from the block's world position or an equivalent per-block seed.

Vary these properties between blocks:

- Shift the broad colour-noise pattern.
- Change the number and positions of blade clusters within the specified ranges.
- Vary blade lengths between 3 and 7 pixels.
- Vary blade angles and the balance of rising-left and rising-right marks.
- Change the density of small gaps.
- Move and resize broad colour patches.
- Vary the positions of rare pale tips.
- Slightly vary the balance between the main base colours.

Keep the same palette and overall density character across neighbouring blocks. Do not create abrupt changes in average brightness or saturation at block boundaries.

Noise and scatter should not reset visibly at each block edge. Use world-coordinate sampling for broad variation where practical, while allowing block-seeded scatter to change individual blade placements.

If the generator cannot sample noise continuously across block boundaries, use overlapping or offset sampling so adjacent blocks do not produce matching edge patterns.

Do not simply rotate or mirror one finished texture to create variation. Repeated clusters and identical arrangements will remain noticeable.

## Must never look like

- Fluorescent lime-green grass.
- Plastic, glossy, or rubbery artificial turf.
- A uniform carpet of identical blades.
- Long, thin, bright streaks.
- A regular checkerboard or visible 64 × 64 block grid.
- A dark, cracked surface with green marks inside it.
- Moss, leafy ground cover, or a field of tiny flowers.
- Photographic grass or realistic individual grass photography.
- A texture whose detail overwhelms the angular, faceted terrain.

## Generator recipe

1. **Set up the palette.** Use only the 12 listed colours. Assign the main ground colours to the base layer, with darker greens for recesses and lighter greens for blade marks.

2. **Generate broad ground variation.** Sample low-frequency value noise over the 64 × 64 texture. Map its values through a colour ramp dominated by #466A4C and #507650, with restrained use of the darker and lighter base colours. Keep the result irregular and softly stepped.

3. **Break up large uniform areas.** Add 5–12 irregular patches, approximately 8–24 pixels across. Blend their colours into the underlying noise with stepped pixel boundaries. Avoid circular spots, parallel bands, or repeated shapes.

4. **Place blade clusters.** Scatter approximately 8–14 cluster centres across the block. Give each cluster a footprint of 5–10 pixels wide and 5–9 pixels tall. Use a variable number of blades per cluster, with loose shared bases and separated tips.

5. **Draw individual blades.** Place approximately 55–90 short angled blades in total. Each blade should be 3–7 pixels long and 1–2 pixels wide. Vary angle, length, colour, and position. Prefer short stepped strokes over perfectly straight lines. Ensure some blades remain visible outside the main clusters.

6. **Add small ground gaps.** Scatter approximately 18–35 irregular dark gaps, each 1–3 pixels across. Bias their placement toward cluster bases and the spaces between clusters. Avoid connected dark networks.

7. **Add rare highlights.** Place approximately 4–10 pale tip marks, each 1–2 pixels across. Use #709362 sparingly and #7B9A69 very rarely. Keep the highlights attached to blades where possible.

8. **Add mixed-green flecks.** Scatter approximately 6–14 small flecks, each 2–5 pixels across, using #394F45, #526653, and #68765A. Keep them subtle enough that the texture still reads as grass.

9. **Build the height map separately from colour.** Start with the ground in levels 8–13, with modest noise variation. Lower selected gaps to levels 4–9. Raise ordinary blade marks to levels 12–17, selected tips to levels 17–21, and only a few compact tuft tips to levels 22–25. Do not let colour patches alone raise the height.

10. **Limit tall features.** Inspect the height map for connected, thin high-value marks. Reduce the height of long blade segments and retain elevated values only on short sections or isolated tips. Keep the highest features compact so they do not form streaks inside neighbouring materials.

11. **Create the side texture.** Reuse the palette and broad colour approach, but reduce blade and cluster density. Keep the majority of the side face as ground colour. Add only a few short marks near its upper edge, avoiding long vertical strokes and horizontal striping.

12. **Vary neighbouring blocks.** Derive noise offsets and scatter placement from world coordinates or a deterministic block seed. Change cluster positions, blade angles, mark counts, and broad colour patterns. Where possible, make the broad noise continuous across block boundaries.

13. **Check the texture at native size.** View the complete 64 × 64 texture without enlargement. Confirm that the blades read as short angled marks, the clusters remain distinct, and the ground does not collapse into a uniform green field.

14. **Check repeated blocks.** Tile several generated blocks in a grid. Look for repeated clusters, matching corners, obvious seams, and changes in average brightness. Adjust noise sampling and scatter variation until the block pattern is difficult to identify.

15. **Check the faceted ground.** Inspect the material on angular terrain under dawn, noon, dusk, and moonlight. Confirm that the texture supports the ground's shape, remains recognisably cool green, and does not become lime, nearly black, or excessively saturated.

16. **Check material blending.** Place meadow grass beside the other materials. Confirm that its raised tips remain occasional, that high-height marks do not form thin streaks, and that the texture does not create a continuous raised border along material transitions.