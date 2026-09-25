// hud.cpp
//
// The UI pass (Section 4.6): text and quads on the font atlas, the
// crosshair, hotbar, toasts, the F3 profiler overlay, and every menu
// screen drawn from its layout. Moved out of game.cpp in M0.11, unchanged.
// Game layer (5).

#include "game_internal.h"

// =======================================================================
// Section 4.6 - UI pass support: quad builders on top of render.h's
// font-glyph atlas layout (UIVertex, the font bands, UI_WHITE_H live
// there -- InitTextures/InitD3D need them too, this
// file only needs the drawing functions built on top).
// =======================================================================

// Nearest baked text size for a requested scale (1.0 == 28px cells).
int UIBandForScale(float scale) {
    float want = 28.0f * scale;
    int best = 0;
    for (int i = 1; i < UI_FONT_BAND_COUNT; i++) {
        float d = UI_BAND_CELL_H[i] - want, dBest = UI_BAND_CELL_H[best] - want;
        if (d * d < dBest * dBest) best = i;
    }
    return best;
}

// Glyph cell `cell` (code-32) of font band `band`. The tiny inset only
// keeps float error from ever flooring into the next cell; at 1:1 with
// point sampling every screen pixel lands on a texel centre anyway.
void UIGlyphRect(const UIFontBand& fb, int cell, float& u0, float& v0, float& u1, float& v1) {
    float texW = (float)UIAtlasWidth(), texH = (float)UIAtlasHeight();
    float x = (float)((cell % UI_ATLAS_COLS) * fb.cellW);
    float y = (float)(fb.atlasY + (cell / UI_ATLAS_COLS) * fb.cellH);
    const float e = 1.0f / 64.0f;
    u0 = (x + e) / texW;               u1 = (x + fb.cellW - e) / texW;
    v0 = (y + e) / texH;               v1 = (y + fb.cellH - e) / texH;
}

int UICharCell(char c) {
    if (c < 32 || c > 126) return -1;
    return (int)c - 32;
}

void UIAddQuad(std::vector<UIVertex>& v, float x0, float y0, float x1, float y1,
                       float u0, float v0, float u1, float v1,
                       float r, float g, float b, float a) {
    v.push_back({ x0, y0, u0, v0, r, g, b, a });
    v.push_back({ x1, y0, u1, v0, r, g, b, a });
    v.push_back({ x1, y1, u1, v1, r, g, b, a });
    v.push_back({ x0, y0, u0, v0, r, g, b, a });
    v.push_back({ x1, y1, u1, v1, r, g, b, a });
    v.push_back({ x0, y1, u0, v1, r, g, b, a });
}

// Untextured tinted rectangle. All four corners sample the middle of the
// atlas's solid-white strip, so the fill is uniform right to its edges.
// (It used to stretch a whole white glyph cell, whose 1px transparent
// rim smeared into a wide fade around every panel and the menu dim.)
void UIDrawRect(std::vector<UIVertex>& v, float x0, float y0, float x1, float y1,
                        float r, float g, float b, float a) {
    float u = 0.5f * UI_WHITE_H / UIAtlasWidth(), vv = 0.5f * UI_WHITE_H / UIAtlasHeight();
    UIAddQuad(v, x0, y0, x1, y1, u, vv, u, vv, r, g, b, a);
}

float UITextWidth(const std::string& text, float scale) {
    return (float)(text.size() * UIGetFontBand(UIBandForScale(scale)).advance);
}
float UITextHeight(float scale) {
    return (float)UIGetFontBand(UIBandForScale(scale)).cellH;
}

// Fixed-advance (monospace) text at the nearest baked size, drawn 1:1 on
// whole pixels -- see render.h. Quads are a padded cell wide but step by
// the font's advance, so neighbouring quads overlap only in their
// transparent padding.
void UIDrawText(std::vector<UIVertex>& v, const std::string& text, float x, float y,
                        float scale, float r, float g, float b, float a) {
    UIFontBand fb = UIGetFontBand(UIBandForScale(scale));
    float curX = floorf(x + 0.5f) - UI_GLYPH_PAD, top = floorf(y + 0.5f);
    for (char c : text) {
        int cell = UICharCell(c);
        if (cell > 0) { // space (cell 0) has no ink
            float u0, v0, u1, v1;
            UIGlyphRect(fb, cell, u0, v0, u1, v1);
            UIAddQuad(v, curX, top, curX + fb.cellW, top + fb.cellH, u0, v0, u1, v1, r, g, b, a);
        }
        curX += fb.advance;
    }
}

void ShowToast(const std::string& message, float seconds) {
    g_toastMessage = message;
    g_toastTimer = seconds;
}

// Draws a run of UI vertices through the UI pipeline, splitting it into
// pieces that fit g_uiVB (whole quads only) rather than truncating --
// a dense menu like Keybindings can exceed one buffer's worth of text.
void UIDrawBatch(const UIVertex* verts, size_t count, ID3D11ShaderResourceView* srv) {
    if (count == 0) return;
    const size_t PIECE = (UI_VB_CAPACITY / 6) * 6;
    UINT stride = sizeof(UIVertex), offset = 0;
    g_context->IASetVertexBuffers(0, 1, &g_uiVB, &stride, &offset);
    g_context->PSSetShaderResources(0, 1, &srv);
    for (size_t start = 0; start < count; start += PIECE) {
        size_t n = std::min(PIECE, count - start);
        D3D11_MAPPED_SUBRESOURCE mapped;
        g_context->Map(g_uiVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, verts + start, n * sizeof(UIVertex));
        g_context->Unmap(g_uiVB, 0);
        g_context->Draw((UINT)n, 0);
    }
}

// Second pass: orthographic-in-pixel-space, depth off, alpha blend on
// (Section 4.6). Builds the crosshair, hotbar, "click to play" hint and
// pause menu as CPU-side quad lists, then draws them through the UI
// pipeline set up in InitD3D. Assumes the world pass already ran this
// frame (so g_context's shader/IA state gets fully re-set here rather
// than assumed).
void RenderUIPass() {
    static std::vector<UIVertex> glyphVerts; // font atlas + white cell (panels, borders, text, crosshair); reused every frame
    glyphVerts.clear();

    bool menuIsOpen = g_menuScreen != MenuScreen::None;

    if (g_mouseCaptured && !menuIsOpen) {
        float cx = g_screenW / 2.0f, cy = g_screenH / 2.0f;
        UIDrawRect(glyphVerts, cx - 8, cy - 1, cx + 8, cy + 1, 1, 1, 1, 0.85f);
        UIDrawRect(glyphVerts, cx - 1, cy - 8, cx + 1, cy + 8, 1, 1, 1, 0.85f);
    }

    // Every placeable block is a plain textured cube, so all hotbar icons
    // sample the one block atlas and share a single batch (drawn between
    // the HUD and menu glyph runs -- see the end of this function).
    // The hotbar: ten slots the player fills from the block library (E).
    if (g_player.hotbarIndex < 0 || g_player.hotbarIndex >= HOTBAR_SLOTS) g_player.hotbarIndex = 0; // an older save's index
    static std::vector<UIVertex> iconVerts;
    iconVerts.clear();
    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        UiRect sr = HotbarSlotRect(g_screenW, g_screenH, i);
        float x0 = sr.x0, x1 = sr.x1, y0 = sr.y0, y1 = sr.y1;
        bool selected = (i == g_player.hotbarIndex);
        if (selected) UIDrawRect(glyphVerts, x0 - 4, y0 - 4, x1 + 4, y1 + 4, 1.0f, 0.9f, 0.2f, 0.9f);
        UIDrawRect(glyphVerts, x0, y0, x1, y1, 0.12f, 0.12f, 0.12f, 0.75f);
        float iu0, iv0, iu1, iv1;
        IconRect(g_hotbar[i], iu0, iv0, iu1, iv1);
        // 32px icon = exactly half the 64px tile, so point sampling keeps
        // every other texel evenly instead of an irregular 64->36 pick.
        UIAddQuad(iconVerts, x0 + 8, y0 + 8, x1 - 8, y1 - 8, iu0, iv0, iu1, iv1, 1, 1, 1, 1);
        // The slot's key, small in the corner: 1-9, then 0.
        char key[2] = { (char)(i == 9 ? '0' : '1' + i), 0 };
        UIDrawText(glyphVerts, key, x0 + 3, y0 + 2, 0.5f, 1, 1, 1, 0.55f);
    }
    const float hbY0 = HotbarSlotRect(g_screenW, g_screenH, 0).y0;

    if (!menuIsOpen) {
        std::string name = g_blocks[g_hotbar[g_player.hotbarIndex]].name;
        for (char& ch : name) ch = ch == '_' ? ' ' : (char)toupper((unsigned char)ch); // "stone_slab" -> "STONE SLAB"
        float scale = 0.8f;
        float tw = UITextWidth(name, scale);
        UIDrawText(glyphVerts, name, (g_screenW - tw) / 2.0f, hbY0 - 26.0f, scale, 1, 1, 1, 0.9f);
    }

    if (!g_mouseCaptured && !menuIsOpen) {
        std::string hint = "CLICK TO PLAY";
        float scale = 1.3f;
        float tw = UITextWidth(hint, scale);
        UIDrawText(glyphVerts, hint, (g_screenW - tw) / 2.0f, g_screenH * 0.42f, scale, 1, 1, 1, 0.9f);
    }

    // Toggle-to-move indicator (Accessibility, Section 11): a small arrow
    // cross in the bottom-left corner, each arrow lit while that direction
    // is latched, so a still-active latch is never invisible. Static --
    // it changes only when the player presses something.
    if (g_toggleMovement && !menuIsOpen && g_gameState == GameState::InGame) {
        const float S = 26.0f, G = 3.0f, x0 = 16.0f;
        const float yTop = g_screenH - 16.0f - (3.0f * S + 2.0f * G);
        struct Cell { GameAction act; const char* glyph; float cx, cy; };
        const Cell cells[4] = {
            { ACT_FORWARD, "^", 1, 0 }, { ACT_LEFT, "<", 0, 1 },
            { ACT_RIGHT, ">", 2, 1 },   { ACT_BACK, "v", 1, 2 },
        };
        UIDrawText(glyphVerts, "TOGGLE MOVE", x0, yTop - 20.0f, 0.65f, 1, 1, 1, 0.75f);
        for (const Cell& c : cells) {
            float cx0 = x0 + c.cx * (S + G), cy0 = yTop + c.cy * (S + G);
            bool on = g_moveToggleLatch[c.act];
            if (on) UIDrawRect(glyphVerts, cx0, cy0, cx0 + S, cy0 + S, 1.0f, g_highContrastUI ? 0.9f : 0.8f, g_highContrastUI ? 0.0f : 0.2f, 0.95f);
            else UIDrawRect(glyphVerts, cx0, cy0, cx0 + S, cy0 + S, 0.08f, 0.08f, 0.08f, g_highContrastUI ? 0.9f : 0.55f);
            float gs = 0.8f;
            float gx = cx0 + (S - UITextWidth(c.glyph, gs)) / 2.0f, gy = cy0 + (S - UITextHeight(gs)) / 2.0f;
            if (on) UIDrawText(glyphVerts, c.glyph, gx, gy, gs, 0, 0, 0, 1);
            else UIDrawText(glyphVerts, c.glyph, gx, gy, gs, 0.7f, 0.7f, 0.7f, g_highContrastUI ? 1.0f : 0.8f);
        }
    }

    // High-contrast mode (Accessibility, Section 11) pushes every panel/
    // button/track toward the luminance extremes -- near-black
    // backgrounds, a strongly saturated hover/handle color -- rather
    // than the subtle gray-shade steps used otherwise. Text is already
    // white-on-dark in both modes, at effectively maximum contrast, so
    // only the fill colors below need to branch.
    // A mechanical key: a raised cap on a darker base. Hovered, the cap
    // brightens; pressed, it sinks into the base (its lit top edge gone, a
    // shadow along its top), the label going down with it; released, it
    // springs back and the button acts. All flat rectangles: free.
    auto drawRowButton = [&](const UIRect& r, const std::string& label, float scale = 1.0f) {
        bool hover = PointInRect(g_mouseX, g_mouseY, r);
        bool held = g_pressActive && PointInRect(g_pressX, g_pressY, r);
        if (held) { g_pressRect = r; g_pressRectValid = true; }
        bool pressed = held && hover;
        const float h = r.y1 - r.y0;
        const float depth = std::max(2.0f, std::min(5.0f, floorf(h * 0.12f)));
        const float sink = pressed ? depth - 1.0f : 0.0f;
        const float capY0 = r.y0 + sink, capY1 = r.y1 - depth + sink;
        float face, lip, base, shadow;
        if (g_highContrastUI) {
            face = pressed ? 0.75f : hover ? 0.9f : 0.06f; lip = hover ? 1.0f : 0.35f; base = 0.0f; shadow = 0.0f;
        } else {
            face = pressed ? 0.27f : hover ? 0.35f : 0.27f; lip = hover ? 0.50f : 0.40f; base = 0.16f; shadow = 0.13f;
        }
        float blue = g_highContrastUI ? (hover || pressed ? 0.1f : face) : 0.06f;
        if (!g_highContrastUI) UIDrawRect(glyphVerts, r.x0 - 1, r.y0 + sink - 1, r.x1 + 1, r.y1 + 1, 0.04f, 0.04f, 0.05f, 1); // outline
        UIDrawRect(glyphVerts, r.x0, r.y0 + depth, r.x1, r.y1, base, base, base + (g_highContrastUI ? 0.0f : 0.03f), 1); // the base / body
        UIDrawRect(glyphVerts, r.x0, capY0, r.x1, capY1, face, face, face + blue, 1);                                    // the cap
        if (pressed) UIDrawRect(glyphVerts, r.x0, capY0, r.x1, capY0 + 2.0f, shadow, shadow, shadow + 0.02f, 1);         // pressed in: shadow along the top
        else UIDrawRect(glyphVerts, r.x0, capY0, r.x1, capY0 + 2.0f, lip, lip, lip + (g_highContrastUI ? 0.0f : 0.05f), 1); // raised: a lit top edge
        UIDrawRect(glyphVerts, r.x0, capY1 - 1.0f, r.x1, capY1, base + 0.05f, base + 0.05f, base + 0.08f, 1);          // the cap's lower edge
        float lw = UITextWidth(label, scale);
        float tr = g_highContrastUI && (hover || pressed) ? 0.0f : 1.0f;
        UIDrawText(glyphVerts, label, r.x0 + ((r.x1 - r.x0) - lw) / 2.0f, capY0 + (capY1 - capY0 - UITextHeight(scale)) / 2.0f, scale, tr, tr, tr, 1);
    };
    // A slider row: label above, track+handle below. Value/range/label
    // text all come from the generic slider-by-ID lookups, so adding a
    // slider anywhere else only means adding cases there, not another
    // copy of this drawing code.
    auto drawSliderRow = [&](UIRect r, int sliderId) {
        UIDrawRect(glyphVerts, r.x0, r.y0, r.x1, r.y1, g_highContrastUI ? 0.03f : 0.16f, g_highContrastUI ? 0.03f : 0.16f, g_highContrastUI ? 0.03f : 0.19f, 1);
        UIDrawText(glyphVerts, GetSliderLabel(sliderId), r.x0 + 8, r.y0 + 2.0f, 0.8f, 1, 1, 1, 1);

        // A bead on a string: a taut thread across the track, brighter on the
        // side the bead has travelled, knotted at both ends; the bead is a
        // round (stacked-rectangle) disc with a lit crown, brighter under the
        // cursor and a touch bigger while it's being dragged.
        UIRect track = GetSliderTrackRect(r);
        SliderRange rng = GetSliderRange(sliderId);
        float t = (GetSliderValue(sliderId) - rng.minV) / (rng.maxV - rng.minV);
        float bx = track.x0 + t * (track.x1 - track.x0);
        float cy = floorf((track.y0 + track.y1) * 0.5f);
        bool hover = PointInRect(g_mouseX, g_mouseY, GetSliderHitRect(r));
        bool dragging = g_draggingSlider == sliderId;
        const bool hc = g_highContrastUI;
        float slack = hc ? 0.55f : 0.30f, taut = hc ? 1.0f : 0.62f;
        UIDrawRect(glyphVerts, track.x0, cy - 1, track.x1, cy + 1, slack, slack, slack + (hc ? 0.0f : 0.04f), 1); // the string
        UIDrawRect(glyphVerts, track.x0, cy - 1, bx, cy + 1, taut, taut * (hc ? 0.9f : 0.95f), hc ? 0.0f : taut * 0.8f, 1); // travelled
        for (float kx : { track.x0, track.x1 }) UIDrawRect(glyphVerts, kx - 2, cy - 3, kx + 2, cy + 3, slack, slack, slack, 1); // knots
        const float R = dragging ? 9.0f : 8.0f;
        float bead = hover || dragging ? 1.0f : (hc ? 0.95f : 0.82f);
        float br = bead, bg = bead * (hc ? 0.9f : 0.9f), bb = hc ? 0.0f : bead * 0.55f;
        for (int k = 0; k < 8; k++) { // a disc in eight horizontal slices
            float y0 = cy - R + k * (2 * R / 8), y1 = y0 + 2 * R / 8;
            float yc = (y0 + y1) * 0.5f - cy;
            float half = sqrtf(std::max(0.0f, R * R - yc * yc));
            float shade = k < 2 ? 1.12f : k > 5 ? 0.78f : 1.0f; // lit crown, shaded underside
            UIDrawRect(glyphVerts, bx - half, y0, bx + half, y1, std::min(1.0f, br * shade), std::min(1.0f, bg * shade), std::min(1.0f, bb * shade), 1);
        }
        UIDrawRect(glyphVerts, bx - R * 0.45f, cy - R * 0.7f, bx - R * 0.05f, cy - R * 0.35f, 1, 1, hc ? 0.6f : 0.95f, hc ? 0.9f : 0.55f); // a glint
    };
    auto drawPanelTitle = [&](const UIRect& panel, float panelW, const char* title, float scale) {
        float tw = UITextWidth(title, scale);
        UIDrawText(glyphVerts, title, panel.x0 + (panelW - tw) / 2.0f, panel.y0 + 16.0f, scale, 1, 1, 1, 1);
    };
    auto drawPanelBg = [&](const UIRect& panel) {
        if (g_highContrastUI) UIDrawRect(glyphVerts, panel.x0, panel.y0, panel.x1, panel.y1, 0.0f, 0.0f, 0.0f, 0.98f);
        else UIDrawRect(glyphVerts, panel.x0, panel.y0, panel.x1, panel.y1, 0.10f, 0.10f, 0.13f, 0.95f);
    };

    // Everything before this point is HUD; menus, FPS and toasts after it
    // must draw over the hotbar icons, which are a separate texture batch.
    size_t hudVertCount = glyphVerts.size();

    // A flat, even dim behind any menu (uniform to the screen edges now
    // that UIDrawRect no longer fades its borders -- no vignette).
    if (g_menuScreen != MenuScreen::None && g_menuScreen != MenuScreen::Library) {
        UIDrawRect(glyphVerts, 0, 0, (float)g_screenW, (float)g_screenH, 0, 0, 0, 0.45f);
    }

    // Block library (library.h): the dim stops short of the hotbar, which
    // stays bright as the drop target. Its icons and the one being dragged
    // go in their own batches after the menu glyphs, so the panel can't
    // cover them.
    static std::vector<UIVertex> libIconVerts, dragIconVerts;
    libIconVerts.clear(); dragIconVerts.clear();
    if (g_menuScreen == MenuScreen::Library) {
        const int count = g_placeableList.count;
        LibraryLayout L = ComputeLibraryLayout(g_screenW, g_screenH, count);
        g_libScroll = std::max(0, std::min(g_libScroll, L.rows - L.visibleRows));
        float hotbarTop = HotbarSlotRect(g_screenW, g_screenH, 0).y0 - 8.0f;
        UIDrawRect(glyphVerts, 0, 0, (float)g_screenW, hotbarTop, 0, 0, 0, 0.45f);
        UIDrawRect(glyphVerts, L.panel.x0, L.panel.y0, L.panel.x1, L.panel.y1, 0.10f, 0.10f, 0.13f, 0.95f);
        UIDrawText(glyphVerts, "BLOCK LIBRARY", L.panel.x0 + 16, L.panel.y0 + 12, 1.0f, 0.9f, 0.9f, 1.0f, 1.0f);
        int hover = LibraryCellAt(L, count, g_libScroll, (float)g_mouseX, (float)g_mouseY);
        for (int i = g_libScroll * L.columns; i < std::min(count, (g_libScroll + L.visibleRows) * L.columns); i++) {
            UiRect c = LibraryCellRect(L, g_libScroll, i);
            bool hot = i == hover || i == g_libGesture.pressed;
            UIDrawRect(glyphVerts, c.x0 + 3, c.y0 + 3, c.x1 - 3, c.y1 - 3, hot ? 0.30f : 0.16f, hot ? 0.28f : 0.16f, hot ? 0.18f : 0.19f, 0.95f);
            float iu0, iv0, iu1, iv1;
            IconRect(g_placeableList.ids[i], iu0, iv0, iu1, iv1);
            UIAddQuad(libIconVerts, c.x0 + 12, c.y0 + 12, c.x1 - 12, c.y1 - 12, iu0, iv0, iu1, iv1, 1, 1, 1, 1);
        }
        if (L.rows > L.visibleRows) {
            char more[48]; snprintf(more, sizeof(more), "ROWS %d-%d OF %d (WHEEL)", g_libScroll + 1, g_libScroll + L.visibleRows, L.rows);
            UIDrawText(glyphVerts, more, L.panel.x1 - 16 - UITextWidth(more, 0.6f), L.panel.y0 + 16, 0.6f, 0.7f, 0.7f, 0.8f, 0.9f);
        }
        int named = g_libGesture.pressed >= 0 ? g_libGesture.pressed : hover;
        std::string label = named >= 0 ? g_blocks[g_placeableList.ids[named]].name
                                       : "CLICK A BLOCK TO USE IT - DRAG IT ONTO A SLOT TO KEEP IT";
        for (char& ch : label) ch = ch == '_' ? ' ' : (char)toupper((unsigned char)ch);
        float ls = named >= 0 ? 0.85f : 0.6f;
        UIDrawText(glyphVerts, label, (g_screenW - UITextWidth(label, ls)) / 2.0f, L.panel.y1 + 6, ls, 1, 1, 1, 0.9f);
        // The dragged block follows the cursor; the slot it would land in lights up.
        if (g_libGesture.dragging && g_libGesture.pressed >= 0) {
            int slot = HotbarSlotAt(g_screenW, g_screenH, (float)g_mouseX, (float)g_mouseY);
            if (slot >= 0) {
                UiRect sr = HotbarSlotRect(g_screenW, g_screenH, slot);
                UIDrawRect(glyphVerts, sr.x0 - 4, sr.y0 - 4, sr.x1 + 4, sr.y1 + 4, 0.4f, 0.9f, 1.0f, 0.6f);
            }
            float iu0, iv0, iu1, iv1;
            IconRect(g_placeableList.ids[g_libGesture.pressed], iu0, iv0, iu1, iv1);
            float mx = (float)g_mouseX, my = (float)g_mouseY;
            UIAddQuad(dragIconVerts, mx - 16, my - 16, mx + 16, my + 16, iu0, iv0, iu1, iv1, 1, 1, 1, 0.9f);
        }
    }



    if (g_menuScreen == MenuScreen::Pause) {
        UIRect panel = SubmenuPanelRect(PAUSE_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, PAUSE_LAYOUT.panelW, "PAUSED", 1.3f);

        drawRowButton(SubmenuRowRect(PAUSE_LAYOUT, PROW_RESUME), "RESUME");
        drawRowButton(SubmenuRowRect(PAUSE_LAYOUT, PROW_OPTIONS), "OPTIONS");
        drawRowButton(SubmenuRowRect(PAUSE_LAYOUT, PROW_SAVE), "SAVE GAME");
        drawRowButton(SubmenuRowRect(PAUSE_LAYOUT, PROW_LOAD), "LOAD GAME");
        drawRowButton(SubmenuRowRect(PAUSE_LAYOUT, PROW_QUIT_TO_TITLE), "QUIT TO TITLE");
        drawRowButton(SubmenuRowRect(PAUSE_LAYOUT, PROW_QUIT), "QUIT");
    } else if (g_menuScreen == MenuScreen::OptionsHub) {
        UIRect panel = SubmenuPanelRect(OPTIONS_HUB_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, OPTIONS_HUB_LAYOUT.panelW, "OPTIONS", 1.2f);

        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_LOOK), "LOOK SETTINGS");
        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_GRAPHICS), "GRAPHICS SETTINGS");
        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_DISPLAY), "DISPLAY SETTINGS");
        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_AUDIO), "AUDIO SETTINGS");
        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_ACCESSIBILITY), "ACCESSIBILITY");
        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_KEYBINDS), "KEYBINDINGS");
        drawRowButton(SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::TitleMain) {
        UIRect panel = SubmenuPanelRect(TITLE_LAYOUT);
        drawPanelBg(panel);
        std::string gameTitle = "VOXISTICS";
        float titleScale = 1.6f;
        UIDrawText(glyphVerts, gameTitle, panel.x0 + (TITLE_LAYOUT.panelW - UITextWidth(gameTitle, titleScale)) / 2.0f, panel.y0 - 60.0f, titleScale, 1, 1, 1, 1);

        drawRowButton(SubmenuRowRect(TITLE_LAYOUT, TROW_NEW_GAME), "NEW GAME");
        drawRowButton(SubmenuRowRect(TITLE_LAYOUT, TROW_LOAD_GAME), "LOAD GAME");
        drawRowButton(SubmenuRowRect(TITLE_LAYOUT, TROW_OPTIONS), "OPTIONS");
        drawRowButton(SubmenuRowRect(TITLE_LAYOUT, TROW_QUIT), "QUIT");
    } else if (g_menuScreen == MenuScreen::SlotPicker) {
        UIRect panel = SubmenuPanelRect(SLOT_PICKER_LAYOUT);
        drawPanelBg(panel);
        const char* title = g_slotPickerMode == SlotPickerMode::New ? "NEW GAME - CHOOSE A SLOT" : "LOAD GAME - CHOOSE A SLOT";
        drawPanelTitle(panel, SLOT_PICKER_LAYOUT.panelW, title, 0.9f);

        for (int slot = 0; slot < MAX_SAVE_SLOTS; slot++) {
            bool occupied = SlotExists(slot);
            std::string label;
            char slotNum[16];
            snprintf(slotNum, sizeof(slotNum), "WORLD %d", slot + 1);
            if (g_slotPickerMode == SlotPickerMode::New && g_confirmOverwriteSlot == slot) {
                label = std::string(slotNum) + " - CLICK AGAIN TO OVERWRITE";
            } else {
                label = std::string(slotNum) + (occupied ? " - SAVED" : " - EMPTY");
                if (g_gameState == GameState::InGame && slot == g_currentSlot) label += " (CURRENT)";
            }
            drawRowButton(SubmenuRowRect(SLOT_PICKER_LAYOUT, slot), label, 0.85f);
        }
        drawRowButton(SubmenuRowRect(SLOT_PICKER_LAYOUT, SLOTROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::LookSettings) {
        UIRect panel = SubmenuPanelRect(LOOK_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, LOOK_LAYOUT.panelW, "LOOK SETTINGS", 1.1f);

        drawRowButton(SubmenuRowRect(LOOK_LAYOUT, LROW_INVERT_X), g_invertX ? "INVERT X LOOK: ON" : "INVERT X LOOK: OFF");
        drawSliderRow(SubmenuRowRect(LOOK_LAYOUT, LROW_SENS_X), SLIDER_SENS_X);
        drawRowButton(SubmenuRowRect(LOOK_LAYOUT, LROW_INVERT_Y), g_invertY ? "INVERT Y LOOK: ON" : "INVERT Y LOOK: OFF");
        drawSliderRow(SubmenuRowRect(LOOK_LAYOUT, LROW_SENS_Y), SLIDER_SENS_Y);
        drawRowButton(SubmenuRowRect(LOOK_LAYOUT, LROW_RESET), "RESET TO DEFAULT");
        drawRowButton(SubmenuRowRect(LOOK_LAYOUT, LROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::Graphics) {
        UIRect panel = SubmenuPanelRect(GRAPHICS_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, GRAPHICS_LAYOUT.panelW, "GRAPHICS SETTINGS", 1.0f);

        drawSliderRow(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_RENDER_DIST), SLIDER_RENDER_DIST);
        drawSliderRow(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_FRAME_LIMIT), SLIDER_FRAME_LIMIT);
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_VSYNC), g_vsync ? "VSYNC: ON" : "VSYNC: OFF");
        // An effect whose shader didn't compile here says so (shader_errors.txt
        // has the details) instead of a toggle that silently does nothing.
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_SHADOWS), !ShadowsAvailable() ? "SUN SHADOWS: UNAVAILABLE" : g_shadows ? "SUN SHADOWS: ON" : "SUN SHADOWS: OFF");
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_OUTLINES), !PostEffectsAvailable() ? "EDGE OUTLINES: UNAVAILABLE" : g_postEdges ? "EDGE OUTLINES: ON" : "EDGE OUTLINES: OFF");
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_SSAO), !PostEffectsAvailable() ? "SCREEN-SPACE AO: UNAVAILABLE" : g_postSSAO ? "SCREEN-SPACE AO: ON" : "SCREEN-SPACE AO: OFF");
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_BLOOM), !BloomAvailable() ? "GLOW (BLOOM): UNAVAILABLE" : g_bloom ? "GLOW (BLOOM): ON" : "GLOW (BLOOM): OFF");
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_RESET), "RESET TO DEFAULT");
        drawRowButton(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::Display) {
        UIRect panel = SubmenuPanelRect(DISPLAY_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, DISPLAY_LAYOUT.panelW, "DISPLAY SETTINGS", 1.0f);

        drawRowButton(SubmenuRowRect(DISPLAY_LAYOUT, DROW_SHOW_FPS), g_showFPS ? "SHOW FPS COUNTER: ON" : "SHOW FPS COUNTER: OFF");
        drawRowButton(SubmenuRowRect(DISPLAY_LAYOUT, DROW_SHOW_PROFILER), g_showProfiler ? "PROFILER (F3): ON" : "PROFILER (F3): OFF");
        drawRowButton(SubmenuRowRect(DISPLAY_LAYOUT, DROW_FULLSCREEN), g_fullscreen ? "FULLSCREEN (F11): ON" : "FULLSCREEN (F11): OFF");
        drawRowButton(SubmenuRowRect(DISPLAY_LAYOUT, DROW_RESET), "RESET TO DEFAULT");
        drawRowButton(SubmenuRowRect(DISPLAY_LAYOUT, DROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::Audio) {
        UIRect panel = SubmenuPanelRect(AUDIO_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, AUDIO_LAYOUT.panelW, "AUDIO SETTINGS", 1.0f);

        drawSliderRow(SubmenuRowRect(AUDIO_LAYOUT, AROW_MASTER_VOLUME), SLIDER_MASTER_VOLUME);
        drawSliderRow(SubmenuRowRect(AUDIO_LAYOUT, AROW_MUSIC_VOLUME), SLIDER_MUSIC_VOLUME);
        drawSliderRow(SubmenuRowRect(AUDIO_LAYOUT, AROW_WORLD_VOLUME), SLIDER_WORLD_VOLUME);
        drawRowButton(SubmenuRowRect(AUDIO_LAYOUT, AROW_RESET), "RESET TO DEFAULT");
        drawRowButton(SubmenuRowRect(AUDIO_LAYOUT, AROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::Accessibility) {
        UIRect panel = SubmenuPanelRect(ACCESSIBILITY_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, ACCESSIBILITY_LAYOUT.panelW, "ACCESSIBILITY", 1.0f);

        drawSliderRow(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_FOV), SLIDER_FOV);
        drawRowButton(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_TOGGLE_MOVE), g_toggleMovement ? "TOGGLE-TO-MOVE: ON" : "TOGGLE-TO-MOVE: OFF");
        drawRowButton(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_HIGH_CONTRAST), g_highContrastUI ? "HIGH-CONTRAST UI: ON" : "HIGH-CONTRAST UI: OFF");
        drawSliderRow(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_MUSIC_INTENSITY), SLIDER_MUSIC_INTENSITY);
        drawRowButton(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_MONO), g_monoAudio ? "MONO AUDIO: ON" : "MONO AUDIO: OFF");
        drawRowButton(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_RESET), "RESET TO DEFAULT");
        drawRowButton(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_BACK), "BACK");
    } else if (g_menuScreen == MenuScreen::Keybindings) {
        UIRect panel = SubmenuPanelRect(KEYBIND_LAYOUT);
        drawPanelBg(panel);
        drawPanelTitle(panel, KEYBIND_LAYOUT.panelW, "KEYBINDINGS", 1.0f);
        std::string hint = "CLICK A ROW, THEN PRESS THE NEW INPUT";
        UIDrawText(glyphVerts, hint, panel.x0 + (KEYBIND_LAYOUT.panelW - UITextWidth(hint, 0.65f)) / 2.0f, panel.y0 + 44.0f, 0.65f, 0.8f, 0.8f, 0.8f, 0.8f);

        for (int i = 0; i < ACT_COUNT; i++) {
            std::string label;
            if (g_rebindingAction == i) label = std::string(g_actionLabels[i]) + (i == ACT_MENU ? ": PRESS INPUT (ESC = ESCAPE)" : ": PRESS INPUT (ESC CANCELS)");
            else label = std::string(g_actionLabels[i]) + ": [" + GetInputDisplayName(g_keyBindings[i]) + "]";
            drawRowButton(SubmenuRowRect(KEYBIND_LAYOUT, i), label, 0.7f);
        }
        drawRowButton(SubmenuRowRect(KEYBIND_LAYOUT, ACT_COUNT), "RESET TO DEFAULT");
        drawRowButton(SubmenuRowRect(KEYBIND_LAYOUT, ACT_COUNT + 1), "BACK");
    }

    if (ProfCapturing()) { // Ctrl+F3 recording: a quiet countdown, bottom left
        char buf[48];
        snprintf(buf, sizeof(buf), "RECORDING PERFORMANCE %d", (int)ceilf(ProfCaptureSecondsLeft()));
        UIDrawText(glyphVerts, buf, 12.0f, g_screenH - 30.0f, 0.6f, 1.0f, 0.55f, 0.45f, 0.9f);
    }
    if (g_showFPS) {
        char buf[32];
        snprintf(buf, sizeof(buf), "FPS: %d", g_fpsDisplay);
        UIDrawText(glyphVerts, buf, 12.0f, 12.0f, 0.9f, 1, 1, 0.6f, 0.9f);
    }

    // Profiler overlay (Part XVI): per-system CPU ms, average and worst
    // over the last ~2 s, then load counters. Monospace, so printf
    // padding lines the columns up.
    if (g_showProfiler) {
        const ProfReport& r = ProfGetReport();
        const float scale = 0.65f, lineH = UITextHeight(scale);
        std::vector<std::string> lines;
        char buf[96];
        snprintf(buf, sizeof(buf), "%-15s %6s %6s", "MS", "AVG", "WORST"); lines.push_back(buf);
        snprintf(buf, sizeof(buf), "%-15s %6.2f %6.2f", "FRAME", r.frameAvgMs, r.frameMaxMs); lines.push_back(buf);
        snprintf(buf, sizeof(buf), "%-15s %6.2f %6.2f", "WORK (NO VSYNC)", r.workAvgMs, r.workMaxMs); lines.push_back(buf);
        for (int i = 0; i < PROF_COUNT; i++) {
            snprintf(buf, sizeof(buf), "  %-13s %6.2f %6.2f", ProfSectionName((ProfSection)i), r.avgMs[i], r.maxMs[i]);
            lines.push_back(buf);
        }
        lines.push_back("");
        snprintf(buf, sizeof(buf), "%-15s %6s %6s", "COUNT", "NOW", "PEAK"); lines.push_back(buf);
        for (int i = 0; i < PCOUNT_COUNT; i++) {
            snprintf(buf, sizeof(buf), "%-15s %6lld %6lld", ProfCounterName((ProfCounter)i),
                     (long long)r.counters[i], (long long)r.countersMax[i]);
            lines.push_back(buf);
        }
        lines.push_back("");
        lines.push_back(ProfBootSummary(false)); // how long start-up took, and on what
        if (g_gameState == GameState::InGame) {
        }
        // The world sound palette's three axes (docs/SOUND_PALETTE.md 3).
        if (g_gameState == GameState::InGame) {
            SoundAxes ax = WorldSoundAxes();
            auto bar = [](float v01) {
                std::string b(11, '-');
                int i = (int)(v01 * 10.0f + 0.5f);
                b[i < 0 ? 0 : i > 10 ? 10 : i] = '#';
                return b;
            };
            lines.push_back("");
            lines.push_back(std::string("FACING ") + CompassPoint(g_player.yaw));
            lines.push_back("SOUNDSCAPE");
            lines.push_back("  NEGATIVE " + bar(0.5f + 0.5f * ax.positive) + " POSITIVE");
            lines.push_back("  CALM     " + bar(ax.activity) + " ACTIVE");
            lines.push_back("  ORGANIC  " + bar(ax.mechanical) + " MECHANICAL");
        }
        float x = 12.0f, y = g_showFPS ? 44.0f : 12.0f;
        float w = 0;
        for (const std::string& l : lines) w = std::max(w, UITextWidth(l, scale));
        UIDrawRect(glyphVerts, x - 6, y - 4, x + w + 6, y + lines.size() * lineH + 4, 0, 0, 0, 0.6f);
        for (const std::string& l : lines) {
            UIDrawText(glyphVerts, l, x, y, scale, 0.85f, 1.0f, 0.85f, 1.0f);
            y += lineH;
        }
    }


    // Transient save/load confirmation -- fades over its last half
    // second so it doesn't just vanish abruptly.
    if (g_toastTimer > 0.0f) {
        float alpha = g_toastTimer < 0.5f ? g_toastTimer / 0.5f : 1.0f;
        float scale = 1.1f;
        float tw = UITextWidth(g_toastMessage, scale);
        UIDrawText(glyphVerts, g_toastMessage, (g_screenW - tw) / 2.0f, 40.0f, scale, 1.0f, 0.95f, 0.55f, alpha);
    }

    g_context->OMSetDepthStencilState(g_uiDepthState, 0);
    float blendFactor[4] = { 0, 0, 0, 0 };
    g_context->OMSetBlendState(g_uiBlendState, blendFactor, 0xFFFFFFFF);
    g_context->VSSetShader(g_uiVS, nullptr, 0);
    g_context->PSSetShader(g_uiPS, nullptr, 0);
    g_context->IASetInputLayout(g_uiLayout);
    g_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_context->VSSetConstantBuffers(0, 1, &g_uiCBuffer);
    g_context->PSSetSamplers(0, 1, &g_uiSampler);

    {
        D3D11_MAPPED_SUBRESOURCE mapped;
        g_context->Map(g_uiCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        float* f = (float*)mapped.pData;
        f[0] = (float)g_screenW; f[1] = (float)g_screenH; f[2] = 0; f[3] = 0;
        g_context->Unmap(g_uiCBuffer, 0);
    }

    UIDrawBatch(glyphVerts.data(), hudVertCount, g_uiSRV);
    UIDrawBatch(iconVerts.data(), iconVerts.size(), g_iconSRV);
    UIDrawBatch(glyphVerts.data() + hudVertCount, glyphVerts.size() - hudVertCount, g_uiSRV);
    if (!libIconVerts.empty()) UIDrawBatch(libIconVerts.data(), libIconVerts.size(), g_iconSRV);
    if (!dragIconVerts.empty()) UIDrawBatch(dragIconVerts.data(), dragIconVerts.size(), g_iconSRV);

    // Restore world-pass defaults so next frame's world draws don't
    // inherit UI blend/depth state.
    g_context->OMSetBlendState(nullptr, blendFactor, 0xFFFFFFFF);
    g_context->OMSetDepthStencilState(g_depthState, 0);
}
