static void TestJetStreamBounds()
{
    constexpr double sampleStepDays = 10.0 / (24.0 * 60.0);
    constexpr int samples = 200 * 24 * 6;

    bool sawNegative = false;
    bool sawPositive = false;

    float previous = JetStreamAngle(0.0);

    CHECK(previous >= -1.45f && previous <= 1.45f);

    for (int i = 1; i <= samples; ++i)
    {
        const double T = i * sampleStepDays;
        const float angle = JetStreamAngle(T);

        CHECK(angle >= -1.45f && angle <= 1.45f);
        CHECK(std::fabs(angle - previous) < 0.01f);

        if (angle < 0.0f)
            sawNegative = true;
        if (angle > 0.0f)
            sawPositive = true;

        previous = angle;
    }

    CHECK(sawNegative);
    CHECK(sawPositive);
}