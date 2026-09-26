static void TestHillsHeights()
{
    const uint64_t seedA = 0x123456789ABCDEF0ULL;
    const uint64_t seedB = 0x0FEDCBA987654321ULL;

    // Determinism and valid height range across 1,000 columns.
    for (int i = 0; i < 1000; ++i)
    {
        const int wx = (i % 100) - 50;
        const int wz = (i / 100) - 5;

        const int h1 = HillsHeight(seedA, wx, wz);
        const int h2 = HillsHeight(seedA, wx, wz);

        CHECK(h1 == h2);
        CHECK(h1 >= 1 && h1 <= 70);
        CHECK(h2 >= 1 && h2 <= 70);
    }

    // Seed variation and neighbouring-column continuity in a 32 x 32 area.
    bool foundSeedDifference = false;

    const int baseX = -16;
    const int baseZ = -16;

    for (int z = 0; z < 32; ++z)
    {
        for (int x = 0; x < 32; ++x)
        {
            const int wx = baseX + x;
            const int wz = baseZ + z;

            const int hA = HillsHeight(seedA, wx, wz);
            const int hB = HillsHeight(seedB, wx, wz);

            CHECK(hA >= 1 && hA <= 70);
            CHECK(hB >= 1 && hB <= 70);

            if (hA != hB)
                foundSeedDifference = true;

            if (x + 1 < 32)
            {
                const int hRight = HillsHeight(seedA, wx + 1, wz);
                const int diff = hA - hRight;
                CHECK(diff >= -20 && diff <= 20);
            }

            if (z + 1 < 32)
            {
                const int hDown = HillsHeight(seedA, wx, wz + 1);
                const int diff = hA - hDown;
                CHECK(diff >= -20 && diff <= 20);
            }
        }
    }

    CHECK(foundSeedDifference);
}