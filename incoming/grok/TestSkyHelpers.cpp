static void TestSkyHelpers()
{
    // SkySmooth: endpoints, outside values, and midpoint.
    CHECK(SkySmooth(0.0f, 1.0f, -0.1f) == 0.0f);
    CHECK(SkySmooth(0.0f, 1.0f, 0.0f) == 0.0f);
    CHECK(SkySmooth(0.0f, 1.0f, 0.5f) == 0.5f);
    CHECK(SkySmooth(0.0f, 1.0f, 1.0f) == 1.0f);
    CHECK(SkySmooth(0.0f, 1.0f, 1.1f) == 1.0f);

    // SkyAngle: equal, opposite, and perpendicular unit vectors.
    const float pi = 3.14159265358979323846f;
    const float angleTolerance = 1.0e-5f;

    const Vec3 xAxis{1.0f, 0.0f, 0.0f};
    const Vec3 negativeXAxis{-1.0f, 0.0f, 0.0f};
    const Vec3 yAxis{0.0f, 1.0f, 0.0f};

    CHECK(std::fabs(SkyAngle(xAxis, xAxis)) <= angleTolerance);
    CHECK(std::fabs(SkyAngle(xAxis, negativeXAxis) - pi) <= angleTolerance);
    CHECK(std::fabs(SkyAngle(xAxis, yAxis) - (pi * 0.5f)) <= angleTolerance);

    // A slightly oversized dot product can arise from rounding.
    // These nearly parallel vectors have a dot product just past 1
    // in ordinary float arithmetic, so acos must be protected from NaN.
    const Vec3 almostParallelA{1.0f, 0.0f, 0.0f};
    const Vec3 almostParallelB{1.0f + 1.0e-7f, 0.0f, 0.0f};

    const float roundedAngle = SkyAngle(almostParallelA, almostParallelB);
    CHECK(std::isfinite(roundedAngle));
    CHECK(std::fabs(roundedAngle) <= angleTolerance);
}