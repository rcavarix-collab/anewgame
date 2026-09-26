static void TestFacePairs()
{
    bool seen[15] = {};

    for (int a = 0; a < 6; ++a)
    {
        for (int b = a + 1; b < 6; ++b)
        {
            const int bit = FacetPairBit(a, b);

            CHECK(bit >= 0 && bit < 15);

            if (bit >= 0 && bit < 15)
            {
                CHECK(!seen[bit]);
                seen[bit] = true;
            }

            CHECK(FacetPairBit(a, b) == FacetPairBit(b, a));
        }
    }

    for (int bit = 0; bit < 15; ++bit)
    {
        CHECK(seen[bit]);
    }

    for (int a = 0; a < 6; ++a)
    {
        CHECK(FacetFacesSee(0, a, a));

        for (int b = 0; b < 6; ++b)
        {
            if (a != b)
            {
                CHECK(!FacetFacesSee(0, a, b));
            }

            CHECK(FacetFacesSee(FACET_ALL_OPEN, a, b));
        }
    }
}