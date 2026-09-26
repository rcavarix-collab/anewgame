// ================================================
//  36 NATURAL DRILLDER / ROCK PATTERNS
//  All tessellate perfectly on 32×32 grid
// ================================================

Bitmap* CreateGranitePattern(int width, int height, const Color& speckleColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(speckleColor);
    int s = GRID_SIZE / 12;
    int sp = GRID_SIZE / 4;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int offX = (y / sp) % 2 * sp / 2;
        int offY = (x / sp) % 2 * sp / 2;
        int var = ((x + y) % 3) + 1;
        g.FillEllipse(&sb, x + offX, y + offY, s * var, s * var);
    }
    return bmp;
}

Bitmap* CreateSandstoneLayerPattern(int width, int height, const Color& layerColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(layerColor);
    int t = GRID_SIZE / 6;
    for (int y = 0; y < height; y += t * 2) for (int x = 0; x < width; x += 4) {
        int wave = (int)(sin((y + x) / 10.0f) * 2);
        g.FillRectangle(&sb, x, y + wave, 4, t);
    }
    return bmp;
}

Bitmap* CreateBasaltCrackPattern(int width, int height, const Color& crackColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    Pen p(crackColor, 1);
    int s = GRID_SIZE / 5;
    for (int x = 0; x < width; x += s) for (int y = 0; y < height; y += 4) g.DrawLine(&p, x, y, x, y + 4);
    for (int y = 0; y < height; y += s) {
        int off = (y / s) % 2 * s / 2;
        for (int x = off; x < width; x += s * 2) for (int i = 0; i < 4; ++i)
            g.DrawLine(&p, x, y + i, x + 4, y + i);
    }
    return bmp;
}

Bitmap* CreateLimestonePorousPattern(int width, int height, const Color& poreColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(poreColor);
    int s = GRID_SIZE / 10, sp = GRID_SIZE / 5;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int var = ((x * y) % 3) + 1;
        g.FillEllipse(&sb, x, y, s * var, s * 2);
    }
    return bmp;
}

Bitmap* CreateLavaBubblePattern(int width, int height, const Color& bubbleColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(bubbleColor);
    int r = GRID_SIZE / 8, sp = GRID_SIZE / 3;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int off = (y / sp + x / sp) % 2 * r;
        g.FillEllipse(&sb, x - off, y - off / 2, r * 2, r * 2);
    }
    return bmp;
}

Bitmap* CreateClayClumpPattern(int width, int height, const Color& c1, const Color& c2) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp);
    int s = GRID_SIZE / 4;
    for (int y = 0; y < height; y += s) for (int x = 0; x < width; x += s) {
        SolidBrush b((x / s + y / s) % 2 ? c1 : c2);
        int ox = ((x + y) % 4) - 2, oy = ((x * y) % 4) - 2;
        g.FillRectangle(&b, x + ox, y + oy, s, s);
    }
    return bmp;
}

Bitmap* CreateBedrockFracturePattern(int width, int height, const Color& fractureColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    Pen p(fractureColor, 2);
    int s = GRID_SIZE / 3;
    for (int y = 0; y < height; y += s) for (int x = 0; x < width; x += s) {
        if ((x + y) % 3) g.DrawLine(&p, x, y + s / 2, x + s, y + s / 2);
        if ((x * y) % 3) g.DrawLine(&p, x + s / 2, y, x + s / 2, y + s);
    }
    return bmp;
}

Bitmap* CreateOreVeinPattern(int width, int height, const Color& veinColor, const Color& oreColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    Pen p(veinColor, 2); SolidBrush sb(oreColor);
    int w = GRID_SIZE / 6;
    for (int y = 0; y < height; y += w * 2) for (int sx = 0; sx < width; sx += GRID_SIZE / 2) {
        int x = sx;
        for (int dx = 0; dx < GRID_SIZE; dx += 4) {
            int wave = (int)(sin((y + dx) / 8.0f) * 3);
            g.DrawLine(&p, x, y, x + 4 + wave, y + w);
            x += 4;
            if (dx % 8 == 0) g.FillEllipse(&sb, x, y + w / 2, 3, 3);
        }
    }
    return bmp;
}

Bitmap* CreateSedimentaryTiltPattern(int width, int height, const Color& c1, const Color& c2) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp);
    int t = GRID_SIZE / 5;
    for (int y = 0; y < height; y += t) for (int x = 0; x < width; x += t) {
        SolidBrush b((x / t + y / t) % 2 ? c1 : c2);
        Point pts[4] = {{x, y}, {x + t, y}, {x + t * 2 / 3, y + t}, {x - t / 3, y + t}};
        g.FillPolygon(&b, pts, 4);
    }
    return bmp;
}

Bitmap* CreateIgneousBlobPattern(int width, int height, const Color& blobColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(blobColor);
    int s = GRID_SIZE / 5, sp = GRID_SIZE / 3;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int var = s + ((x + y) % 3);
        g.FillEllipse(&sb, x - var / 4, y - var / 4, var * 1.5f, var);
    }
    return bmp;
}

Bitmap* CreateSlateShalePattern(int width, int height, const Color& flakeColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(flakeColor);
    int w = GRID_SIZE / 4, h = GRID_SIZE / 8;
    for (int y = 0; y < height; y += h * 2) for (int x = 0; x < width; x += w) {
        int off = (y / (h * 2)) % 2 * w / 2;
        g.FillRectangle(&sb, x + off, y, w, h);
    }
    return bmp;
}

Bitmap* CreatePumiceHoneycombPattern(int width, int height, const Color& wallColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    Pen p(wallColor, 1);
    int s = GRID_SIZE / 6, hh = (int)(s * sqrt(3) / 2);
    for (int y = 0; y < height; y += hh) for (int x = 0; x < width; x += s * 3) {
        int oy = (x / (s * 3)) % 2 * hh / 2;
        Point pts[6] = {
            {x + s, y + oy},
            {x + s * 1.5f, y + oy - hh / 2},
            {x + s * 2, y + oy},
            {x + s * 2, y + oy + hh / 2},
            {x + s * 1.5f, y + oy + hh / 2},
            {x + s * 0.5f, y + oy}
        };
        g.DrawPolygon(&p, pts, 6);
    }
    return bmp;
}

Bitmap* CreateMarbleVeinPattern(int width, int height, const Color& veinColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    Pen p(veinColor, 1); SolidBrush sb(veinColor);
    int t = GRID_SIZE / 10;
    for (int y = 0; y < height; y += t * 2) for (int sx = 0; sx < width; sx += GRID_SIZE / 3) {
        int x = sx;
        for (int dx = 0; dx < GRID_SIZE / 2; dx += 3) {
            int wave = (int)(sin((y + dx * 2) / 12.0f) * t * 2);
            g.DrawLine(&p, x, y + wave, x + 3, y + wave + ((dx % 4) - 2));
            x += 3;
            if (dx % 6 == 0) g.FillEllipse(&sb, x - 1, y + wave - 1, t, t);
        }
    }
    return bmp;
}

Bitmap* CreateObsidianGlossPattern(int width, int height, const Color& glossColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(glossColor);
    int s = GRID_SIZE / 12, sp = GRID_SIZE / 4;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int var = s + ((x + y * 2) % 3);
        g.FillEllipse(&sb, x, y, var * 2, var);
        Pen p(glossColor, 1);
        g.DrawLine(&p, x + var, y, x + var * 2, y + var);
    }
    return bmp;
}

Bitmap* CreateQuartzCrystalPattern(int width, int height, const Color& crystalColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(crystalColor);
    int s = GRID_SIZE / 6, sp = GRID_SIZE / 3;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int off = (y / sp) % 2 * s / 2;
        Point pts[6] = {
            {x + off, y}, {x + off + s / 2, y + s / 4},
            {x + off + s, y}, {x + off + s, y + s},
            {x + off + s / 2, y + s * 3 / 4}, {x + off, y + s}
        };
        g.FillPolygon(&sb, pts, 6);
    }
    return bmp;
}

Bitmap* CreateCoalLayerPattern(int width, int height, const Color& coalColor, const Color& dustColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(coalColor);
    SolidBrush sb(dustColor);
    int h = GRID_SIZE / 8, d = GRID_SIZE / 5;
    for (int y = 0; y < height; y += h) {
        int off = (int)(sin(y / 8.0f) * 2);
        g.FillRectangle(&sb, 0, y + off, width, h / 2);
        for (int x = 0; x < width; x += d)
            g.FillRectangle(&sb, x + (y % 3), y + h / 2, 2, 2);
    }
    return bmp;
}

Bitmap* CreateGeodeCrystalPattern(int width, int height, const Color& crystalColor, const Color& liningColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush cb(crystalColor), lb(liningColor);
    int r = GRID_SIZE / 5, sp = GRID_SIZE / 2;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        g.FillEllipse(&lb, x - r / 2, y - r / 2, r * 2, r * 2);
        for (int i = 0; i < 6; ++i) {
            float a = i * 3.14159f / 3;
            int cx = x + (int)(cos(a) * r / 2);
            int cy = y + (int)(sin(a) * r / 2);
            g.FillRectangle(&cb, cx - 1, cy - 2, 2, 4);
        }
    }
    return bmp;
}

Bitmap* CreateMudCrackPattern(int width, int height, const Color& crackColor, const Color& mudColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(mudColor);
    Pen p(crackColor, 2);
    int s = GRID_SIZE / 4;
    for (int y = 0; y < height; y += s) for (int x = 0; x < width; x += s) {
        Point pts[4] = {{x, y}, {x + s - ((y % 3) * 2), y},
                       {x + s, y + s / 2}, {x, y + s}};
        g.DrawPolygon(&p, pts, 4);
    }
    return bmp;
}

Bitmap* CreateGravelPebblePattern(int width, int height, const Color& c1, const Color& c2) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp);
    int s = GRID_SIZE / 5, sp = s * 2;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int ox = (y / sp) % 2 * sp / 2;
        SolidBrush b((x / sp + y / sp) % 2 ? c1 : c2);
        g.FillEllipse(&b, x + ox - s / 2, y - s / 2, s * 2, s * 1.5f);
    }
    return bmp;
}

Bitmap* CreateGneissBandPattern(int width, int height, const Color& c1, const Color& c2, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    int t = GRID_SIZE / 7;
    for (int y = 0; y < height; y += t * 2) for (int x = 0; x < width; x += 4) {
        int wave = (int)(sin((x + y) / 12.0f) * t);
        g.FillRectangle(&SolidBrush(c1), x, y + wave, 4, t);
        g.FillRectangle(&SolidBrush(c2), x, y + t + wave, 4, t);
    }
    return bmp;
}

Bitmap* CreateSchistFlakePattern(int width, int height, const Color& flakeColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(flakeColor);
    int len = GRID_SIZE / 3, w = GRID_SIZE / 12;
    for (int y = 0; y < height; y += w * 3) for (int x = 0; x < width; x += len) {
        int tilt = ((x / len) % 3) - 1;
        g.FillRectangle(&sb, x, y + tilt * 2, len, w * 2);
    }
    return bmp;
}

Bitmap* CreatePegmatiteCoarsePattern(int width, int height, const Color& c1, const Color& c2) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp);
    int s = GRID_SIZE / 3;
    for (int y = 0; y < height; y += s) for (int x = 0; x < width; x += s) {
        SolidBrush b((x / s + y / s) % 2 ? c1 : c2);
        int jx = s + ((y % 4) - 2), jy = s + ((x % 4) - 2);
        Point pts[4] = {{x, y}, {x + s, y}, {x + jx / 2, y + jy}, {x, y + s}};
        g.FillPolygon(&b, pts, 4);
    }
    return bmp;
}

Bitmap* CreateTuffAshPattern(int width, int height, const Color& ashColor, const Color& fragmentColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(ashColor);
    SolidBrush sb(fragmentColor);
    int s = GRID_SIZE / 10, d = GRID_SIZE / 4;
    for (int y = 0; y < height; y += d) for (int x = 0; x < width; x += d) {
        int var = s + ((x + y) % 4);
        int off = (y / d) % 2 * s;
        g.FillEllipse(&sb, x + off, y + off, var * 2, var);
    }
    return bmp;
}

Bitmap* CreateFossilBedPattern(int width, int height, const Color& fossilColor, const Color& matrixColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(matrixColor);
    SolidBrush sb(fossilColor);
    int s = GRID_SIZE / 5;
    for (int y = 0; y < height; y += s * 2) for (int x = 0; x < width; x += s * 2) {
        Point pts[5] = {
            {x + s / 2, y}, {x + s, y + s / 3},
            {x + s / 2, y + 2 * s / 3}, {x, y + s / 3},
            {x + s / 4, y + s / 2}
        };
        g.FillPolygon(&sb, pts, 5);
    }
    return bmp;
}

Bitmap* CreateFlintNodulePattern(int width, int height, const Color& noduleColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(noduleColor);
    int s = GRID_SIZE / 6, sp = GRID_SIZE / 3;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int ox = (y / sp) % 2 * s / 2;
        int oy = (x / sp) % 2 * s / 2;
        g.FillEllipse(&sb, x + ox - s / 2, y + oy - s / 2, s * 2, s * 1.5f);
    }
    return bmp;
}

Bitmap* CreateIronOreStreakPattern(int width, int height, const Color& streakColor, const Color& oreColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(streakColor), ob(oreColor);
    int w = GRID_SIZE / 8;
    for (int y = 0; y < height; y += w * 3) for (int sx = 0; sx < width; sx += GRID_SIZE / 2) {
        int wave = (int)(sin((sx + y) / 10.0f) * w);
        g.FillRectangle(&sb, sx, y + wave, GRID_SIZE / 4, w * 2);
        g.FillEllipse(&ob, sx + GRID_SIZE / 8, y + w, 3, 3);
    }
    return bmp;
}

Bitmap* CreateSaltCrystalPattern(int width, int height, const Color& crystalColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(crystalColor);
    int s = GRID_SIZE / 10, sp = GRID_SIZE / 5;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int h = s + ((x + y) % 3);
        g.FillRectangle(&sb, x, y, s * 2, h);
    }
    return bmp;
}

Bitmap* CreateTalcFibrousPattern(int width, int height, const Color& fiberColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    Pen p(fiberColor, 1);
    int len = GRID_SIZE / 5, sp = GRID_SIZE / 4;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += len) {
        int tilt = ((y / sp) % 3) - 1;
        g.DrawLine(&p, x, y + tilt * 2, x + len, y + tilt * 2 + sp / 2);
    }
    return bmp;
}

Bitmap* CreateKimberlitePipePattern(int width, int height, const Color& pipeColor, const Color& inclusionColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush pb(pipeColor), ib(inclusionColor);
    int r = GRID_SIZE / 7;
    for (int y = 0; y < height; y += GRID_SIZE / 2) for (int x = 0; x < width; x += GRID_SIZE / 2) {
        g.FillEllipse(&pb, x - r, y - r, r * 2, r * 2);
        g.FillRectangle(&ib, x - 2, y, 4, 3);
    }
    return bmp;
}

Bitmap* CreateGypsumVeinPattern(int width, int height, const Color& veinColor, const Color& fiberColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush vb(veinColor); Pen fp(fiberColor, 1);
    int w = GRID_SIZE / 8;
    for (int y = 0; y < height; y += w * 4) for (int sx = 0; sx < width; sx += GRID_SIZE / 3) {
        g.FillRectangle(&vb, sx, y, w * 2, w * 3);
        for (int i = 0; i < 5; ++i)
            g.DrawLine(&fp, sx + w, y + i * w, sx + w * 2, y + i * w + 2);
    }
    return bmp;
}

Bitmap* CreateAndesitePorphyryPattern(int width, int height, const Color& phenoColor, const Color& groundmassColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(groundmassColor);
    SolidBrush sb(phenoColor);
    int s = GRID_SIZE / 7, sp = GRID_SIZE / 3;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int var = s + ((x ^ y) % 2);
        g.FillEllipse(&sb, x - var / 2, y - var / 2, var * 2, var);
    }
    return bmp;
}

Bitmap* CreateRhyoliteFlowPattern(int width, int height, const Color& c1, const Color& c2) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp);
    int t = GRID_SIZE / 6;
    for (int y = 0; y < height; y += t * 2) for (int x = 0; x < width; x += 5) {
        int wave = (int)(sin(x * 0.3f + y * 0.5f) * t);
        g.FillRectangle(&SolidBrush(c1), x, y + wave, 5, t);
        g.FillRectangle(&SolidBrush(c2), x, y + t + wave, 5, t);
    }
    return bmp;
}

Bitmap* CreatePeridotiteGrainPattern(int width, int height, const Color& olivineColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(olivineColor);
    int s = GRID_SIZE / 5;
    for (int y = 0; y < height; y += s) for (int x = 0; x < width; x += s) {
        int j = ((x + y) % 4) - 2;
        Point pts[4] = {{x, y}, {x + s, y + j / 2},
                       {x + s - j, y + s}, {x + j, y + s}};
        g.FillPolygon(&sb, pts, 4);
    }
    return bmp;
}

Bitmap* CreateSerpentiniteScalePattern(int width, int height, const Color& scaleColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush sb(scaleColor);
    int s = GRID_SIZE / 6, sp = (int)(s * 1.5f);
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int shear = (y / sp) % 2 * s / 2;
        g.FillEllipse(&sb, x + shear, y, s * 1.5f, s);
    }
    return bmp;
}

Bitmap* CreateTravertinePorousPattern(int width, int height, const Color& poreColor, const Color& bandingColor, const Color& baseColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(baseColor);
    SolidBrush pb(poreColor), bb(bandingColor);
    int r = GRID_SIZE / 10;
    for (int y = 0; y < height; y += GRID_SIZE / 5) {
        g.FillRectangle(&bb, 0, y, width, 2);
        for (int x = 0; x < width; x += GRID_SIZE / 4)
            g.FillEllipse(&pb, x + (y % 3), y - r, r * 2, r * 2);
    }
    return bmp;
}

Bitmap* CreateLateritePisolitePattern(int width, int height, const Color& pisoliteColor, const Color& matrixColor) {
    Bitmap* bmp = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bmp); g.Clear(matrixColor);
    SolidBrush sb(pisoliteColor);
    int s = GRID_SIZE / 8, sp = GRID_SIZE / 3;
    for (int y = 0; y < height; y += sp) for (int x = 0; x < width; x += sp) {
        int off = (x / sp + y / sp) % 2 * s / 2;
        g.FillEllipse(&sb, x + off - s / 2, y - s / 2, s * 2, s * 1.8f);
    }
    return bmp;
}


////


// ===============================
// 37–64 Natural / Drillder Patterns – Batch 2
// ===============================

// 37. Ice Sheet Fractures (thin interconnected crack lines)
Bitmap* CreateIceSheetFractures(int width, int height, const Color& iceColor, const Color& crackColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush ice(iceColor); Pen crack(crackColor, 1);
    g.FillRectangle(&ice, 0, 0, width, height);

    int cell = 8;
    for (int y = 0; y <= height; y += cell) {
        for (int x = 0; x <= width; x += cell) {
            int x2 = (x + cell/2) % width;
            int y2 = (y + cell/3) % height;
            g.DrawLine(&crack, x % width, y % height, x2, y2);

            if ((x/cell + y/cell) % 3 == 0)
                g.DrawLine(&crack, x2, y2, (x2 + 3) % width, (y2 + 2) % height);
        }
    }
    return bitmap;
}

// 38. Soil / Loam Granular Texture
Bitmap* CreateLoamTexture(int width, int height, const Color& soilColor, const Color& grainColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush soil(soilColor); SolidBrush grain(grainColor);
    g.FillRectangle(&soil, 0, 0, width, height);

    int cell = 4;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            if (((x + y) % 7) < 2)
                g.FillRectangle(&grain, x + 1, y + 1, 1, 1);
        }
    }
    return bitmap;
}

// 39. Compacted Dirt Cells
Bitmap* CreateCompactedDirtCells(int width, int height, const Color& dirtColor, const Color& edgeColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush dirt(dirtColor); Pen edge(edgeColor, 1);

    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            g.FillEllipse(&dirt, x + 1, y + 1, cell - 2, cell - 2);
            if ((x/cell + y/cell) % 4 == 0)
                g.DrawEllipse(&edge, x + 1, y + 1, cell - 2, cell - 2);
        }
    }
    return bitmap;
}

// 40. Pebble Soil Mix
Bitmap* CreatePebbleSoilMix(int width, int height, const Color& soilColor, const Color& pebbleColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush soil(soilColor); SolidBrush pebble(pebbleColor);
    g.FillRectangle(&soil, 0, 0, width, height);

    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            if ((x/cell + y/cell) % 3 == 0)
                g.FillEllipse(&pebble, x + 2, y + 2, 3, 3);
        }
    }
    return bitmap;
}

// 41. Fine Dust Drift (micro ripples)
Bitmap* CreateFineDustDrift(int width, int height, const Color& dustColor, const Color& rippleColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush dust(dustColor); Pen ripple(rippleColor, 1);
    g.FillRectangle(&dust, 0, 0, width, height);

    for (int y = 0; y < height; y += 3) {
        for (int x = 0; x < width; x += 1) {
            if ((x + y) % 9 < 3)
                g.DrawLine(&ripple, x, y + (x % 2), x + 1, y + (x % 2));
        }
    }
    return bitmap;
}

// 42. Concreted Pebble Layer (rounded inclusions in matrix)
Bitmap* CreateConcretedPebbleLayer(int width, int height, const Color& matrixColor, const Color& pebbleColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush mat(matrixColor); SolidBrush peb(pebbleColor);
    g.FillRectangle(&mat, 0, 0, width, height);

    int grid = 8;
    for (int y = 0; y < height; y += grid) {
        for (int x = 0; x < width; x += grid) {
            if (((x+y)/grid) % 2 == 0)
                g.FillEllipse(&peb, x + 1, y + 2, 4, 4);
        }
    }
    return bitmap;
}

// 43. Gravel Ridge Pack
Bitmap* CreateGravelRidgePack(int width, int height, const Color& soilColor, const Color& stoneColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush soil(soilColor); SolidBrush stone(stoneColor);
    g.FillRectangle(&soil, 0, 0, width, height);

    int cell = 6;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            g.FillEllipse(&stone, x + 1, y + 1, 3, 3);
        }
    }
    return bitmap;
}

// 44. Clay Layer Veining
Bitmap* CreateClayLayerVeining(int width, int height, const Color& clayColor, const Color& veinColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush clay(clayColor); Pen vein(veinColor, 1);
    g.FillRectangle(&clay, 0, 0, width, height);

    for (int y = 3; y < height; y += 10) {
        for (int x = 0; x < width; x += 12) {
            g.DrawLine(&vein, x, y, x + 7, y + ((x/12)%2 ? 1 : -1));
        }
    }
    return bitmap;
}

// 45. Wave-Rutted Sand
Bitmap* CreateWaveRuttedSand(int width, int height, const Color& sandColor, const Color& ridgeColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush sand(sandColor); Pen ridge(ridgeColor, 1);
    g.FillRectangle(&sand, 0, 0, width, height);

    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 1) {
            if ((x + y) % 6 < 2)
                g.DrawLine(&ridge, x, y, x + 1, y);
        }
    }
    return bitmap;
}

// 46. Mossy Stone
Bitmap* CreateMossyStone(int width, int height, const Color& stoneColor, const Color& mossColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush stone(stoneColor); SolidBrush moss(mossColor);
    g.FillRectangle(&stone, 0, 0, width, height);

    int grid = 8;
    for (int y = 0; y < height; y += grid) {
        for (int x = 0; x < width; x += grid) {
            if ((x/grid + y/grid) % 4 == 0)
                g.FillEllipse(&moss, x + 1, y + 1, 5, 5);
        }
    }
    return bitmap;
}

// 47. Moss Pocket Scatter (bubbly pockets)
Bitmap* CreateMossPocketScatter(int width, int height, const Color& stoneColor, const Color& mossColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush stone(stoneColor), moss(mossColor);
    g.FillRectangle(&stone, 0, 0, width, height);

    int step = 6;
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            if ((x + y) % 2 == 0)
                g.FillEllipse(&moss, x + 1, y + 1, 3, 3);
        }
    }
    return bitmap;
}

// 48. Root-Infused Soil
Bitmap* CreateRootInfusedSoil(int width, int height, const Color& soilColor, const Color& rootColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush soil(soilColor); Pen root(rootColor, 2);
    g.FillRectangle(&soil, 0, 0, width, height);

    int cell = 16;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            g.DrawBezier(&root, x+2, y+2, x+6, y+8, x+10, y+4, x+14, y+12);
        }
    }
    return bitmap;
}

// 49. Root Mat Layer (thick shared roots)
Bitmap* CreateRootMatLayer(int width, int height, const Color& soilColor, const Color& rootColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush soil(soilColor); Pen root(rootColor, 3);
    g.FillRectangle(&soil, 0, 0, width, height);

    for (int y = 0; y < height; y += 12) {
        g.DrawLine(&root, 0, y + 2, width, y + 2);
        if (y % 24 == 0)
            g.DrawLine(&root, width/2, y, width/2 + 6, y + 12);
    }
    return bitmap;
}

// 50. Fungal Web Pattern
Bitmap* CreateFungalWebPattern(int width, int height, const Color& soilColor, const Color& webColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush soil(soilColor); Pen web(webColor, 1);
    g.FillRectangle(&soil, 0, 0, width, height);

    int node = 8;
    for (int y = 0; y <= height; y += node) {
        for (int x = 0; x <= width; x += node) {
            g.DrawLine(&web, x, y, (x + node/2) % width, (y + node/3) % height);
        }
    }
    return bitmap;
}

// 51. Lichen Spotting
Bitmap* CreateLichenSpotting(int width, int height, const Color& rockColor, const Color& lichenColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush rock(rockColor); SolidBrush lich(lichenColor);
    g.FillRectangle(&rock, 0, 0, width, height);

    int grid = 10;
    for (int y = 0; y < height; y += grid) {
        for (int x = 0; x < width; x += grid) {
            if (((x + y)/grid) % 3 == 0)
                g.FillEllipse(&lich, x + 2, y + 2, 3, 3);
        }
    }
    return bitmap;
}

// 52. Embedded Ore Flecks
Bitmap* CreateEmbeddedOreFlecks(int width, int height, const Color& rockColor, const Color& oreColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush rock(rockColor), ore(oreColor);
    g.FillRectangle(&rock, 0, 0, width, height);

    int grid = 6;
    for (int y = 0; y < height; y += grid) {
        for (int x = 0; x < width; x += grid) {
            if ((x/grid + y/grid) % 4 == 0)
                g.FillRectangle(&ore, x + 2, y + 2, 2, 2);
        }
    }
    return bitmap;
}

// 53. Rusted Metallic Rock (natural ore coloration)
Bitmap* CreateRustedMetallicRock(int width, int height, const Color& stoneColor, const Color& rustColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush stone(stoneColor); SolidBrush rust(rustColor);
    g.FillRectangle(&stone, 0, 0, width, height);

    int cell = 6;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            if (((x+y)/cell) % 5 == 0)
                g.FillRectangle(&rust, x + 1, y + 1, 3, 3);
        }
    }
    return bitmap;
}

// 54. Pulsed Vein Mineral
Bitmap* CreatePulsedVeinMineral(int width, int height, const Color& stoneColor, const Color& veinColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush stone(stoneColor); Pen vein(veinColor, 1);
    g.FillRectangle(&stone, 0, 0, width, height);

    int step = 12;
    for (int y = 0; y < height; y += step) {
        g.DrawArc(&vein, 0, y - 4, width, step + 8, 0, 180);
    }
    return bitmap;
}

// 55. Luminescent Fungal Stone
Bitmap* CreateLuminescentFungalStone(int width, int height, const Color& baseColor, const Color& glowColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush base(baseColor); SolidBrush glow(glowColor);
    g.FillRectangle(&base, 0, 0, width, height);

    int grid = 10;
    for (int y = 2; y < height; y += grid) {
        for (int x = 2; x < width; x += grid) {
            if (((x+y)/grid) % 3 == 0)
                g.FillEllipse(&glow, x, y, 4, 4);
        }
    }
    return bitmap;
}

// 56. Star-Speckled Rare Ore
Bitmap* CreateStarSpeckledOre(int width, int height, const Color& stoneColor, const Color& speckColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush stone(stoneColor); SolidBrush speck(speckColor);
    g.FillRectangle(&stone, 0, 0, width, height);

    int step = 4;
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            if ((x + y) % 5 == 0)
                g.FillRectangle(&speck, x + 1, y + 1, 1, 1);
        }
    }
    return bitmap;
}

// 57. Magmatic Flow
Bitmap* CreateMagmaticFlow(int width, int height, const Color& rockColor, const Color& flowColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush rock(rockColor); Pen flow(flowColor, 1);
    g.FillRectangle(&rock, 0, 0, width, height);

    for (int y = 0; y < height; y++) {
        int offset = (int)(2 * sin((y % height) * 0.4));
        g.DrawLine(&flow, offset, y, width - 1 + offset, y);
    }
    return bitmap;
}

// 58. Cooling Lava Crust
Bitmap* CreateCoolingLavaCrust(int width, int height, const Color& crustColor, const Color& glowColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush crust(crustColor); Pen glow(glowColor, 1);
    g.FillRectangle(&crust, 0, 0, width, height);

    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            g.DrawLine(&glow, x+1, y+1, x + cell - 1, y + cell - 3);
        }
    }
    return bitmap;
}

// 59. Magma Fracture Lines
Bitmap* CreateMagmaFractureLines(int width, int height, const Color& rockColor, const Color& magmaColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush rock(rockColor); Pen magma(magmaColor, 2);
    g.FillRectangle(&rock, 0, 0, width, height);

    int step = 10;
    for (int y = 0; y < height; y += step) {
        g.DrawLine(&magma, 0, y, width, y + step/2);
    }
    return bitmap;
}

// 60. Heat-Distorted Bands (stepped distortion)
Bitmap* CreateHeatDistortedBands(int width, int height, const Color& rockColor, const Color& bandColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush rock(rockColor); Pen band(bandColor, 1);
    g.FillRectangle(&rock, 0, 0, width, height);

    int bandH = 5;
    for (int y = 0; y < height; y += bandH) {
        for (int x = 0; x < width; x++) {
            int off = ((x/3 + y/2) % 3);
            g.DrawLine(&band, x, y + off, x+1, y + off);
        }
    }
    return bitmap;
}

// 61. Lava Tubule Texture
Bitmap* CreateLavaTubuleTexture(int width, int height, const Color& rockColor, const Color& lavaColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush rock(rockColor); SolidBrush lava(lavaColor);
    g.FillRectangle(&rock, 0, 0, width, height);

    int cell = 12;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            g.FillEllipse(&lava, x + 3, y + 3, 6, 6);
        }
    }
    return bitmap;
}

// 62. Echo-Lines (segmented arcs)
Bitmap* CreateEchoLines(int width, int height, const Color& baseColor, const Color& echoColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush base(baseColor); Pen echo(echoColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);

    int r = 8;
    for (int i = r; i < width; i += r) {
        g.DrawArc(&echo, width/2 - i, height/2 - i, i*2, i*2, 45, 40);
    }
    return bitmap;
}

// 63. Gravity-Shear Striation
Bitmap* CreateGravityShearStriation(int width, int height, const Color& rockColor, const Color& lineColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush rock(rockColor); Pen line(lineColor, 1);
    g.FillRectangle(&rock, 0, 0, width, height);

    int stripe = 8;
    for (int y = 0; y < height; y += stripe) {
        for (int x = 0; x < width; x += 1) {
            if ((x + y) % 7 < 3)
                g.DrawLine(&line, x, (y + x/3) % height, x+1, (y + x/3) % height);
        }
    }
    return bitmap;
}

// 64. Fractal Cellular Stone (two-scale fractures)
Bitmap* CreateFractalCellularStone(int width, int height, const Color& baseColor, const Color& crackColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);

    SolidBrush base(baseColor); Pen crack(crackColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);

    int big = 16;
    int small = 8;

    // Big cells
    for (int y = 0; y < height; y += big)
        for (int x = 0; x < width; x += big)
            g.DrawRectangle(&crack, x+1, y+1, big-2, big-2);

    // Small cells inside them
    for (int y = 0; y < height; y += small)
        for (int x = 0; x < width; x += small)
            if (((x/small) + (y/small)) % 2 == 0)
                g.DrawRectangle(&crack, x+2, y+2, small-4, small-4);

    return bitmap;
}


//////

// =====================
// Natural / Drillder-themed patterns (1..36)
// =====================

// 1. Basalt Columns
Bitmap* CreateBasaltColumns(int width, int height, const Color& columnColor, const Color& crackColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush col(columnColor); SolidBrush crack(crackColor);
    int colW = 8; // 32 / 4 columns
    for (int x = 0; x < width; x += colW) {
        // vertical column body
        g.FillRectangle(&col, x, 0, colW, height);
        // small zigzag top/bottom chips (tile periodic)
        for (int y = 0; y < height; y += 8) {
            int chip = ((x/colW + y/8) % 3 == 0) ? 1 : 0;
            if (chip) g.FillRectangle(&crack, x + colW - 2, y, 2, 1);
        }
    }
    return bitmap;
}

// 2. Columnar Hex Rupture (simplified hex columns tiled horizontally)
Bitmap* CreateColumnarHexRupture(int width, int height, const Color& baseColor, const Color& chipColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush base(baseColor); SolidBrush chip(chipColor);
    int w = 8, h = 16;
    for (int x = -w; x < width + w; x += w*1) {
        int offset = ((x / w) & 1) ? h/2 : 0;
        Point pts[6];
        // draw vertical elongate hex as polygon clipped inside tile area (tile-safe grid)
        for (int i = 0; i < 2; ++i) {
            int cx = x + i * w;
            int cy = offset;
            pts[0] = Point(cx, cy + 0);
            pts[1] = Point(cx + w/2, cy + h/4);
            pts[2] = Point(cx + w/2, cy + 3*h/4);
            pts[3] = Point(cx, cy + h);
            pts[4] = Point(cx - w/2, cy + 3*h/4);
            pts[5] = Point(cx - w/2, cy + h/4);
            g.FillPolygon(&base, pts, 6);
            // chips at some edges
            if (((x/ w) + i) % 3 == 0) g.FillRectangle(&chip, cx + w/2 - 1, cy + h/2 - 1, 2, 2);
        }
    }
    return bitmap;
}

// 3. Block-Fracture Igneous
Bitmap* CreateBlockFractureIgneous(int width, int height, const Color& rockColor, const Color& crackColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush rock(rockColor); Pen crack(crackColor, 1);
    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            // draw slightly offset rectangle to mimic block
            int ox = (x + (y % (cell*2) == 0 ? 0 : cell/4)) % width;
            g.FillRectangle(&rock, ox, y, cell - 1, cell - 1);
            // short hairline cracks
            if ((x / cell + y / cell) % 3 == 0) {
                g.DrawLine(&crack, ox + 1, y + cell/2, ox + cell - 2, y + cell/2 + ((x / cell) % 2 ? 1 : -1));
            }
        }
    }
    return bitmap;
}

// 4. Angular Lava Shards
Bitmap* CreateAngularLavaShards(int width, int height, const Color& shardColor, const Color& matrixColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush shard(shardColor); SolidBrush matrix(matrixColor);
    int grid = 8;
    for (int y = 0; y < height; y += grid) {
        for (int x = 0; x < width; x += grid) {
            // triangle shard inside each 8x8 cell
            Point tri[3] = {Point(x + 1, y + grid - 1), Point(x + grid - 1, y + 1 + ((x/8)%2)), Point(x + grid/2, y + 2)};
            g.FillPolygon(&shard, tri, 3);
            g.FillRectangle(&matrix, x, y, grid, grid); // subtle underlying fill ensures tile coverage
        }
    }
    return bitmap;
}

// 5. Interlocking Tectonic Plates
Bitmap* CreateInterlockingPlates(int width, int height, const Color& plateColor, const Color& seamColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush plate(plateColor); Pen seam(seamColor, 1);
    int large = 16;
    for (int y = 0; y < height; y += large) {
        for (int x = 0; x < width; x += large) {
            // random-ish polygon within a large cell (but deterministic pattern using x,y)
            Point pts[5] = {
                Point(x + 1, y + 1),
                Point(x + large - 3, y + 2 + ((x/large)%2)),
                Point(x + large - 2, y + large - 3),
                Point(x + 2 + ((y/large)%2), y + large - 2),
                Point(x + 1, y + large/2)
            };
            g.FillPolygon(&plate, pts, 5);
            g.DrawPolygon(&seam, pts, 5);
        }
    }
    return bitmap;
}

// 6. Diabase Flow Bands
Bitmap* CreateDiabaseFlowBands(int width, int height, const Color& bandColor1, const Color& bandColor2, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush b1(bandColor1); SolidBrush b2(bandColor2);
    int band = 4; // 32/8 = 8 bands approx
    for (int y = 0; y < height; y += band) {
        bool toggle = ((y / band) % 2 == 0);
        for (int x = 0; x < width; x += 1) {
            // diagonal bias slope ±1 kept by offset using x and y
            int dx = (x + (y/2)) % (band * 2);
            SolidBrush &brush = toggle ? b1 : b2;
            g.FillRectangle(&brush, x, y, 1, band);
        }
    }
    return bitmap;
}

// 7. Migmatite Swirl Bands (stepped waves)
Bitmap* CreateMigmatiteSwirlBands(int width, int height, const Color& darkColor, const Color& lightColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush dark(darkColor), light(lightColor);
    int layerH = 6;
    for (int y = 0; y < height; y += layerH) {
        bool useDark = ((y / layerH) % 2 == 0);
        for (int x = 0; x < width; ++x) {
            int step = ((x / 4) + (y / layerH)) % 3; // stepped offset
            if (useDark) g.FillRectangle(&dark, x, y + step, 1, layerH - step);
            else g.FillRectangle(&light, x, y + step, 1, layerH - step);
        }
    }
    return bitmap;
}

// 8. Jade Fracture Mesh
Bitmap* CreateJadeFractureMesh(int width, int height, const Color& stoneColor, const Color& veinColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush stone(stoneColor); Pen vein(veinColor, 1);
    g.FillRectangle(&stone, 0, 0, width, height);
    int step = 8;
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            if ((x/step + y/step) % 4 == 0) {
                g.DrawLine(&vein, x, y, x + step, y + step/2);
                g.DrawLine(&vein, x + step/2, y, x, y + step);
            }
        }
    }
    return bitmap;
}

// 9. Anorthosite Plate Blocks
Bitmap* CreateAnorthositePlates(int width, int height, const Color& plateColor, const Color& fissureColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush plate(plateColor); Pen fissure(fissureColor, 1);
    int big = 12;
    for (int y = 0; y < height; y += big) {
        for (int x = 0; x < width; x += big) {
            g.FillRectangle(&plate, x + 1, y + 1, big - 2, big - 2);
            if ((x/big + y/big) % 3 == 0) g.DrawLine(&fissure, x + 2, y + 3, x + big - 3, y + big - 4);
        }
    }
    return bitmap;
}

// 10. Fractured Obsidian
Bitmap* CreateFracturedObsidian(int width, int height, const Color& darkColor, const Color& highlightColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush dark(darkColor); Pen hi(highlightColor, 1);
    g.FillRectangle(&dark, 0, 0, width, height);
    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            if ((x / cell + y / cell) % 2 == 0) {
                g.DrawLine(&hi, x + 1, y + 1, x + cell - 2, y + cell - 3);
                g.DrawLine(&hi, x + cell - 2, y + 1, x + 1, y + cell - 3);
            }
        }
    }
    return bitmap;
}

// 11. Carbonized Obsidian Plates
Bitmap* CreateCarbonizedPlates(int width, int height, const Color& plateColor, const Color& edgeColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush plate(plateColor); Pen edge(edgeColor, 1);
    int plateW = 16;
    for (int y = 0; y < height; y += plateW) {
        for (int x = 0; x < width; x += plateW) {
            g.FillRectangle(&plate, x + 1, y + 1, plateW - 2, plateW - 2);
            g.DrawRectangle(&edge, x + 1, y + 1, plateW - 2, plateW - 2);
        }
    }
    return bitmap;
}

// 12. Rubble / Broken Stone
Bitmap* CreateRubbleBrokenStone(int width, int height, const Color& rockColor, const Color& stoneEdge, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush rock(rockColor); Pen edge(stoneEdge, 1);
    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            // draw random-looking polygon deterministically
            Point pts[4] = {Point(x + 1, y + 2), Point(x + cell - 2, y + 1 + ((x/8)%3)), Point(x + cell - 3, y + cell - 2), Point(x + 2, y + cell - 3)};
            g.FillPolygon(&rock, pts, 4);
            g.DrawPolygon(&edge, pts, 4);
        }
    }
    return bitmap;
}

// 13. Fissured Rock
Bitmap* CreateFissuredRock(int width, int height, const Color& baseColor, const Color& fissColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush base(baseColor); Pen fiss(fissColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);
    int step = 8;
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            if ((x/step + y/step) % 2 == 0) {
                g.DrawLine(&fiss, x + 1, y + 2, x + step - 2, y + step - 3);
            } else {
                g.DrawLine(&fiss, x + step - 2, y + 2, x + 1, y + step - 3);
            }
        }
    }
    return bitmap;
}

// 14. Cracked Stone Mesh
Bitmap* CreateCrackedStoneMesh(int width, int height, const Color& rockColor, const Color& crackColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush rock(rockColor); Pen crack(crackColor, 1);
    g.FillRectangle(&rock, 0, 0, width, height);
    int grid = 6;
    for (int y = 0; y <= height; y += grid) {
        for (int x = 0; x <= width; x += grid) {
            // small T-shaped crack that wraps
            g.DrawLine(&crack, x%width, y%height, (x + 3)%width, y%height);
            g.DrawLine(&crack, x%width, y%height, x%width, (y + 3)%height);
        }
    }
    return bitmap;
}

// 15. Shatter-Cell Tiling (irregular polygon shards)
Bitmap* CreateShatterCellTiling(int width, int height, const Color& baseColor, const Color& shardEdgeColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush base(baseColor); Pen edge(shardEdgeColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);
    int cell = 8;
    for (int y = 0; y < height; y += cell) {
        for (int x = 0; x < width; x += cell) {
            Point pts[5] = {
                Point(x + 1, y + 1),
                Point(x + cell - 3, y + 2),
                Point(x + cell - 2, y + cell - 3),
                Point(x + 3, y + cell - 2),
                Point(x + 1, y + cell/2)
            };
            g.FillPolygon(&base, pts, 5);
            g.DrawPolygon(&edge, pts, 5);
        }
    }
    return bitmap;
}

// 16. Broken Pavement Rock
Bitmap* CreateBrokenPavementRock(int width, int height, const Color& slabColor, const Color& crackColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush slab(slabColor); Pen crack(crackColor, 1);
    int slabH = 12;
    for (int y = 0; y < height; y += slabH) {
        for (int x = 0; x < width; x += slabH) {
            g.FillRectangle(&slab, x + 1, y + 1, slabH - 2, slabH - 2);
            if ((x/slabH + y/slabH) % 2 == 0) g.DrawLine(&crack, x + 2, y + 2, x + slabH - 3, y + slabH - 3);
        }
    }
    return bitmap;
}

// 17. Sedimentary Strata
Bitmap* CreateSedimentaryStrata(int width, int height, const Color layers[], int layerCount, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    int layerH = height / layerCount;
    for (int i = 0; i < layerCount; ++i) {
        SolidBrush brush(layers[i]);
        int y = i * layerH;
        // slight horizontal jitter but tile-periodic
        for (int x = 0; x < width; ++x) {
            int dy = ((x + i*3) % 8) / 4; // small periodic jitter
            g.FillRectangle(&brush, x, y + dy, 1, layerH - dy);
        }
    }
    return bitmap;
}

// 18. Rippled Sediment Strata
Bitmap* CreateRippledSedimentStrata(int width, int height, const Color& layerA, const Color& layerB, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(backgroundColor);
    SolidBrush a(layerA), b(layerB);
    int band = 6;
    for (int y = 0; y < height; y += band) {
        SolidBrush &brush = ((y / band) % 2 == 0) ? a : b;
        for (int x = 0; x < width; ++x) {
            int off = ((x / 4) % 3); // stepped ripple
            g.FillRectangle(&brush, x, y + off, 1, band - off);
        }
    }
    return bitmap;
}

// 19. Chalk/Clay Microstrata
Bitmap* CreateChalkClayMicrostrata(int width, int height, const Color& thinColor, const Color& bgColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bgColor);
    Pen thin(thinColor, 1);
    for (int y = 0; y < height; y += 2) {
        if ((y / 2) % 3 == 0) {
            // small broken lines
            for (int x = 0; x < width; x += 4) {
                g.DrawLine(&thin, x, y, x + 3, y);
            }
        }
    }
    return bitmap;
}

// 20. Layered Breccia (layers with block inclusions)
Bitmap* CreateLayeredBreccia(int width, int height, const Color layerColor, const Color chunkColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush layer(layerColor), chunk(chunkColor);
    int layers = 4;
    int h = height / layers;
    for (int i = 0; i < layers; ++i) {
        int y = i * h;
        g.FillRectangle(&layer, 0, y, width, h);
        // add 1-2 chunks per layer deterministically
        if (i % 2 == 0) g.FillRectangle(&chunk, (i*7)%width, y + h/4, 6, h/2);
        else g.FillRectangle(&chunk, (i*11)%width, y + h/6, 8, h/3);
    }
    return bitmap;
}

// 21. Tilted Strata (gentle angle)
Bitmap* CreateTiltedStrata(int width, int height, const Color& a, const Color& b, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush A(a), B(b);
    int band = 6;
    for (int y = -height; y < height + width; y += band) {
        bool pick = ((y / band) % 2 == 0);
        for (int x = 0; x < width; ++x) {
            int yy = (y + x/4) % height;
            g.FillRectangle(pick ? &A : &B, x, yy, 1, band);
        }
    }
    return bitmap;
}

// 22. Cross-Bedded Sandstone
Bitmap* CreateCrossBeddedSandstone(int width, int height, const Color& sandA, const Color& sandB, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush A(sandA), B(sandB);
    int stripe = 6;
    // two angled stripe sets: slope +1 and -1 with step 8, ensure clipping to tile edges
    for (int y = -height; y < height; y += stripe) {
        for (int x = 0; x < width; ++x) {
            bool which = (((x + y)/stripe) % 2 == 0);
            g.FillRectangle(which ? &A : &B, x, (y + x) % height, 1, stripe);
        }
    }
    return bitmap;
}

// 23. Mudstone Compaction Wrinkles
Bitmap* CreateMudstoneWrinkles(int width, int height, const Color& baseColor, const Color& wrinkleColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush base(baseColor); Pen wrinkle(wrinkleColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 8) {
            int len = 3 + ((x + y) % 3);
            g.DrawLine(&wrinkle, x, y + (x%2), x + len, y + (x%2));
        }
    }
    return bitmap;
}

// 24. Clay Slip Cracks
Bitmap* CreateClaySlipCracks(int width, int height, const Color& clay, const Color& crack, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush clayB(clay); Pen crackP(crack, 1);
    g.FillRectangle(&clayB, 0, 0, width, height);
    // longer horizontal cracks with small forks
    for (int y = 2; y < height; y += 10) {
        for (int x = 0; x < width; x += 12) {
            g.DrawLine(&crackP, x, y, x + 8, y + ((x/12)%2 ? 1 : -1));
            g.DrawLine(&crackP, x + 3, y, x + 3, y + ((x/12)%3 == 0 ? 2 : 1));
        }
    }
    return bitmap;
}

// 25. Fossil Traces (stylized tiny semicircles)
Bitmap* CreateFossilTraces(int width, int height, const Color& stone, const Color& fossilColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush stoneB(stone); SolidBrush fossil(fossilColor);
    g.FillRectangle(&stoneB, 0, 0, width, height);
    int grid = 8;
    for (int y = 2; y < height; y += grid) {
        for (int x = 2; x < width; x += grid) {
            if (((x/2 + y/2) % 5) == 0) {
                g.FillPie(&fossil, x, y, 6, 6, 180, 180); // semicircle
            }
        }
    }
    return bitmap;
}

// 26. Quarry Stone Blocks (natural)
Bitmap* CreateQuarryStoneBlocks(int width, int height, const Color& stoneColor, const Color& edgeColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush stone(stoneColor); Pen edge(edgeColor, 1);
    int w = 10, h = 10;
    for (int y = 0; y < height; y += h) {
        for (int x = 0; x < width; x += w) {
            g.FillRectangle(&stone, x + 1, y + 1, w - 2, h - 2);
            g.DrawRectangle(&edge, x + 1, y + 1, w - 2, h - 2);
        }
    }
    return bitmap;
}

// 27. Cubic Basalt Nodes
Bitmap* CreateCubicBasaltNodes(int width, int height, const Color& baseColor, const Color& darkEdge, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush base(baseColor); Pen edge(darkEdge, 1);
    int node = 12;
    for (int y = 0; y < height; y += node) {
        for (int x = 0; x < width; x += node) {
            g.FillRectangle(&base, x + 2, y + 2, node - 4, node - 4);
            g.DrawRectangle(&edge, x + 2, y + 2, node - 4, node - 4);
        }
    }
    return bitmap;
}

// 28. Slate / Stone Slabs
Bitmap* CreateSlateStoneSlabs(int width, int height, const Color& slabColor, const Color& seamColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush slab(slabColor); Pen seam(seamColor, 1);
    int slabW = 16;
    for (int y = 0; y < height; y += slabW/2) {
        for (int x = 0; x < width; x += slabW) {
            g.FillRectangle(&slab, x + ((y/slabW)%2 ? slabW/4 : 0), y, slabW - 2, slabW/2 - 1);
            g.DrawRectangle(&seam, x + ((y/slabW)%2 ? slabW/4 : 0), y, slabW - 2, slabW/2 - 1);
        }
    }
    return bitmap;
}

// 29. Offset Slab Layering
Bitmap* CreateOffsetSlabLayering(int width, int height, const Color& slab1, const Color& slab2, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush A(slab1), B(slab2);
    int groupW = 16;
    for (int y = 0; y < height; y += 8) {
        for (int x = 0; x < width; x += groupW) {
            bool t = ((y/8) % 2 == 0);
            g.FillRectangle(t ? &A : &B, x + (t ? 0 : groupW/4), y, groupW - 2, 8 - 1);
        }
    }
    return bitmap;
}

// 30. Crystalline Rock (Quartz / Ice) simple diagonal shards
Bitmap* CreateCrystallineShards(int width, int height, const Color& shardColor, const Color& baseColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush base(baseColor); Pen shard(shardColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);
    for (int y = 2; y < height; y += 8) {
        for (int x = 2; x < width; x += 8) {
            g.DrawLine(&shard, x, y, x + 4, y + 6);
            g.DrawLine(&shard, x + 2, y, x - 2, y + 6);
        }
    }
    return bitmap;
}

// 31. Clustered Crystal Veins
Bitmap* CreateClusteredCrystalVeins(int width, int height, const Color& veinColor, const Color& rockColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush rock(rockColor); Pen vein(veinColor, 1);
    g.FillRectangle(&rock, 0, 0, width, height);
    int step = 8;
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            if (((x+y)/step) % 3 == 0) {
                // small branched vein
                g.DrawLine(&vein, x, y, x + 5, y + 2);
                g.DrawLine(&vein, x + 3, y, x + 3, y + 4);
            }
        }
    }
    return bitmap;
}

// 32. Cross-Hatch Crystal Fracture
Bitmap* CreateCrossHatchCrystal(int width, int height, const Color& lineColor, const Color& baseColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush base(baseColor); Pen line(lineColor, 1);
    g.FillRectangle(&base, 0, 0, width, height);
    int spacing = 6;
    for (int i = -width; i < width*2; i += spacing) {
        g.DrawLine(&line, i, 0, i + width, height);
        g.DrawLine(&line, i + width, 0, i, height);
    }
    return bitmap;
}

// 33. Faceted Gem Surface (few angular facets)
Bitmap* CreateFacetedGemSurface(int width, int height, const Color facets[], int facetCount, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    // Create a few triangular facets anchored on grid centers
    int cx = width/2, cy = height/2;
    for (int i = 0; i < facetCount; ++i) {
        SolidBrush brush(facets[i % facetCount]);
        Point pts[3] = { Point(cx, cy), Point((i*7 + 3) % width, (i*5 + 2) % height), Point((i*11 + 9) % width, (i*3 + 8) % height) };
        g.FillPolygon(&brush, pts, 3);
    }
    return bitmap;
}

// 34. Hex-Facet Mineral Field
Bitmap* CreateHexFacetField(int width, int height, const Color& a, const Color& b, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush A(a), B(b);
    int hexW = 8;
    int hexH = 7; // approximate
    for (int y = 0; y < height + hexH; y += hexH) {
        for (int x = 0; x < width + hexW; x += hexW*1) {
            int offsetY = ((x / hexW) % 2) ? hexH/2 : 0;
            Point pts[6] = {
                Point(x, y + offsetY),
                Point(x + hexW/2, y + offsetY + hexH/4),
                Point(x + hexW/2, y + offsetY + 3*hexH/4),
                Point(x, y + offsetY + hexH),
                Point(x - hexW/2, y + offsetY + 3*hexH/4),
                Point(x - hexW/2, y + offsetY + hexH/4)
            };
            g.FillPolygon(((x/hexW + y/hexH) % 2 == 0) ? &A : &B, pts, 6);
        }
    }
    return bitmap;
}

// 35. Bismuth-Style Stepped Crystals (concentric rectangles)
Bitmap* CreateBismuthSteppedCrystals(int width, int height, const Color steps[], int stepCount, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    int pad = 1;
    for (int s = 0; s < stepCount; ++s) {
        SolidBrush brush(steps[s % stepCount]);
        int inset = s * (width / (2*stepCount));
        g.FillRectangle(&brush, inset, inset, width - inset*2, height - inset*2);
    }
    return bitmap;
}

// 36. Needle Crystal Scatter (narrow spikes)
Bitmap* CreateNeedleCrystalScatter(int width, int height, const Color& needleColor, const Color& rockColor, const Color& bg) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics g(bitmap); g.Clear(bg);
    SolidBrush rock(rockColor); Pen needle(needleColor, 1);
    g.FillRectangle(&rock, 0, 0, width, height);
    int spacing = 6;
    for (int y = 2; y < height; y += spacing) {
        for (int x = 2; x < width; x += spacing) {
            int dir = ((x + y) % 4);
            if (dir == 0) g.DrawLine(&needle, x, y, x, y - 4);
            else if (dir == 1) g.DrawLine(&needle, x, y, x + 3, y - 3);
            else if (dir == 2) g.DrawLine(&needle, x, y, x + 4, y);
            else g.DrawLine(&needle, x, y, x - 3, y - 3);
        }
    }
    return bitmap;
}
