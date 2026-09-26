#include "sky.h"

static void TestDiscCoverProperties()
{
    const float radii[] = {0.25f, 0.5f, 1.0f, 2.0f, 5.0f};
    const float distances[] = {
        0.0f, 0.1f, 0.25f, 0.5f, 0.75f,
        1.0f, 1.5f, 2.0f, 3.0f, 5.0f, 10.0f
    };

    constexpr float monotonicTolerance = 1e-5f;
    constexpr float relativeTolerance = 1e-3f;

    // Result must remain within [0, 1], and must not increase
    // as the centre distance grows.
    for (float r1 : radii)
    {
        for (float r2 : radii)
        {
            float previous = DiscCover(r1, r2, distances[0]);

            CHECK(previous >= 0.0f && previous <= 1.0f);

            for (int i = 1; i < static_cast<int>(
                     sizeof(distances) / sizeof(distances[0])); ++i)
            {
                const float current = DiscCover(r1, r2, distances[i]);

                CHECK(current >= 0.0f && current <= 1.0f);
                CHECK(current <= previous + monotonicTolerance);

                previous = current;
            }

            // At and beyond external tangency, the overlap is zero.
            CHECK(DiscCover(r1, r2, r1 + r2) == 0.0f);
            CHECK(DiscCover(r1, r2, r1 + r2 + 1.0f) == 0.0f);
        }
    }

    // The overlap area is invariant under swapping the two discs.
    for (float r1 : radii)
    {
        for (float r2 : radii)
        {
            for (float d : distances)
            {
                const float cover1 = DiscCover(r1, r2, d);
                const float cover2 = DiscCover(r2, r1, d);

                const float area1 = r1 * r1 * cover1;
                const float area2 = r2 * r2 * cover2;
                const float scale = std::max(area1, area2);

                if (scale == 0.0f)
                {
                    CHECK(area1 == 0.0f && area2 == 0.0f);
                }
                else
                {
                    CHECK(std::fabs(area1 - area2) <=
                          relativeTolerance * scale);
                }
            }
        }
    }

    // Coincident equal discs completely cover one another.
    for (float r : radii)
    {
        CHECK(DiscCover(r, r, 0.0f) == 1.0f);
    }
}