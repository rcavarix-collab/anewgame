# Walkgrid World Palette

## Palette principles

This is one coordinated palette for all 12 ground materials.

The palette uses earthy, moderately saturated colours, with enough separation between adjacent materials to keep the world readable. Each material has a dominant base colour, a darker shadow colour and a lighter highlight colour.

The faceted ground remains the main visual feature. Material colours should support the angular terrain rather than compete with it.

Palette design priorities:
- Keep neighbouring materials distinguishable by value, hue, or both.
- Give each material a recognisable colour identity.
- Keep shadows coloured rather than pushing every material toward black.
- Keep highlights within the material's hue family, avoiding chalky or fluorescent extremes.
- Preserve useful colour separation when the scene is illuminated by the sun, moon or sky.
- Allow existing texture marks to provide additional distinction without relying on them to rescue nearly identical base colours.

ASSUMPTION: Hex values describe the texture colours before scene lighting and are specified as ordinary 8-bit sRGB colours.

## World palette

| Material | Base hex | Shadow hex | Highlight hex |
|---|---|---|---|
| Meadow grass | `#78964A` | `#465D32` | `#B2C875` |
| Dry turf | `#A18B4D` | `#665B35` | `#C8B66E` |
| Moss | `#587D43` | `#354F30` | `#91AD65` |
| Dirt | `#80543A` | `#4D352A` | `#B17A50` |
| Loam | `#684B3A` | `#403129` | `#967052` |
| Clay | `#B16D50` | `#704538` | `#D99A73` |
| Sand | `#D2B878` | `#8E7950` | `#EBDDAB` |
| Gravel | `#96958A` | `#5E625E` | `#C3C1B1` |
| Stone | `#888D91` | `#51575D` | `#B8BEC0` |
| Slate | `#737C80` | `#454D52` | `#A5B0B1` |
| Sandstone | `#C58C61` | `#815B43` | `#E0B58A` |
| Snow | `#D9E1E2` | `#899FA9` | `#F5F3E8` |

## Neighbour contrast

The notes below compare the base colours first. Shadow and highlight colours provide additional separation within each material.

### Meadow grass beside dirt

**Base colours:** Meadow grass `#78964A`; dirt `#80543A`.

Contrast: Strong hue separation. Grass is a muted olive green; dirt is a warm, earthy brown. Their similar midrange values are acceptable because their hues differ clearly.

Decision: Keep both colours.

### Meadow grass beside dry turf

**Base colours:** Meadow grass `#78964A`; dry turf `#A18B4D`.

Contrast: Moderate. Both are natural, muted ground colours, but dry turf is lighter and more yellow-brown. The value and hue differences should distinguish them at normal viewing distances.

Decision: Keep both colours. Avoid pushing dry turf toward green, which would make it too similar to meadow grass.

### Meadow grass beside moss

**Base colours:** Meadow grass `#78964A`; moss `#587D43`.

Contrast: Close. Both are green, and their base colours have similar luminance. Their difference is mostly in lightness and saturation.

Proposed fix: Keep moss darker and slightly cooler. Its base is `#587D43`, while meadow grass remains `#78964A`. Preserve this separation in the generator's dominant colour fields rather than relying solely on individual blades or tufts.

Decision: Acceptable with the proposed darker moss colour. If the rendered materials merge under moonlight, move moss toward `#4D713F`.

### Sand beside gravel

**Base colours:** Sand `#D2B878`; gravel `#96958A`.

Contrast: Strong. Sand is warm, light and yellow; gravel is a darker, neutral grey. Their difference is visible in both value and hue.

Decision: Keep both colours. Avoid giving gravel a strong yellow cast.

### Sand beside clay

**Base colours:** Sand `#D2B878`; clay `#B16D50`.

Contrast: Strong. Sand is pale and yellow, while clay is darker and distinctly orange-red. The difference should remain readable even when fine texture marks are not individually resolved.

Decision: Keep both colours. Preserve clay's warm red component and sand's yellow component.

### Stone beside slate

**Base colours:** Stone `#888D91`; slate `#737C80`.

Contrast: Too close for reliable separation by base colour alone. Both are cool grey materials, and their difference is primarily a modest change in value.

Proposed fix: Keep stone lighter and close to neutral, while making slate darker, less blue and slightly greener-neutral than a conventional blue slate. The selected slate base, `#737C80`, is deliberately lighter and less saturated than near-black blue-grey. Stone remains `#888D91`.

Decision: Acceptable with this revised slate palette. If the materials still merge in the rendered scene, lighten stone to `#969A9B` or darken slate to `#687478`, checking that slate does not become nearly black.

### Sandstone beside grass

**Base colours:** Sandstone `#C58C61`; meadow grass `#78964A`.

Contrast: Strong. Sandstone is a warm orange-brown; grass is olive green. The opposing hue families create clear separation.

Decision: Keep both colours. Sandstone should remain warm rather than drifting toward the yellow-green range of dry turf.

## Palette adjustments

The following decisions are particularly important to the overall palette:

1. **Slate must not be nearly black or strongly blue.** Its base is `#737C80`, its shadow is `#454D52`, and its highlight is `#A5B0B1`. This creates a readable grey stone family with a restrained cool tint.

2. **Moss must remain distinct from meadow grass.** Moss uses a darker, slightly cooler green: `#587D43`. If lighting compresses their difference, use the optional darker moss base `#4D713F`.

3. **Stone and slate need a deliberate value hierarchy.** Stone is the lighter, more neutral grey; slate is darker and cooler. Do not independently tune their generators in ways that erase this relationship.

4. **Sand and sandstone must retain different hue identities.** Sand is pale yellow; sandstone is orange-brown. Avoid giving sandstone a pale yellow base simply to make its highlights brighter.

5. **Dirt and loam must remain distinct despite sharing a brown family.** Dirt is lighter and warmer; loam is darker and more subdued.

## Generator integration

Use the listed base, shadow and highlight hex values as the main colour anchors for each material's existing colour-ramp generator.

Suggested process:

1. Convert each hex colour to RGB channel values.
2. Use the shadow, base and highlight colours as the three principal ramp anchors.
3. Let the existing noise and scatter logic vary colours within that material's ramp.
4. Keep local variation subordinate to the material's base identity.
5. Avoid introducing random colours from neighbouring material palettes.
6. Preserve the existing texture height maps and height-based material blending.
7. Inspect adjacent materials together, not only as isolated texture swatches.
8. Check the complete palette under dawn, noon, dusk and moonlight.

ASSUMPTION: The current generator can use these three colours as ramp anchors and interpolate between them. If it uses a different ramp representation, preserve the same colour relationships when mapping the values into its existing format.

## Final neighbour checklist

| Neighbour pair | Contrast assessment | Action |
|---|---|---|
| Meadow grass / dirt | Strong hue separation | Keep |
| Meadow grass / dry turf | Moderate value and hue separation | Keep |
| Meadow grass / moss | Close | Keep darker moss; optional darker base if needed |
| Sand / gravel | Strong value and hue separation | Keep |
| Sand / clay | Strong value and hue separation | Keep |
| Stone / slate | Close | Keep lighter neutral stone and restrained cool-grey slate |
| Sandstone / meadow grass | Strong hue separation | Keep |

## Acceptance criteria

- All 12 materials have explicit base, shadow and highlight hex values.
- Every specified neighbour pair has a contrast assessment.
- The close green pair has a proposed separation strategy.
- The close grey pair has a proposed separation strategy.
- Slate is no longer represented by a nearly black, strongly blue base.
- The palette is implementable using ordinary RGB hex values and a noise-and-scatter colour-ramp generator.
- No changes to geometry, lighting code, height maps or rendering features are required by this palette proposal.