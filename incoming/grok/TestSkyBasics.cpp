static void TestSkyBasics()
{
    for (uint32_t day = 0; day < 3; ++day)
    {
        for (int sample = 0; sample < 24; ++sample)
        {
            const float dayTime = 3600.0f * sample / 23.0f;
            const SkyState sky = ComputeSky(dayTime, day);

            const float sunLength = std::sqrt(
                Dot(sky.sunDir, sky.sunDir));
            const float moonLength = std::sqrt(
                Dot(sky.moonDir, sky.moonDir));

            CHECK(std::fabs(sunLength - 1.0f) <= 0.001f);
            CHECK(std::fabs(moonLength - 1.0f) <= 0.001f);

            CHECK(sky.daylight >= 0.30f);
            CHECK(sky.daylight <= 1.0f);
        }
    }

    const SkyState midday = ComputeSky(1500.0f, 0);
    const SkyState sunsetSide = ComputeSky(3300.0f, 0);

    CHECK(midday.sunDir.y > 0.0f);
    CHECK(sunsetSide.sunDir.y < 0.0f);
    CHECK(midday.starsVisible == 0.0f);

    for (int day = 0; day < 8; ++day)
    {
        const SkyState first = ComputeSky(1800.0f,
                                          static_cast<uint32_t>(day));
        const SkyState repeated = ComputeSky(
            1800.0f, static_cast<uint32_t>(day + 8));

        CHECK(std::fabs(first.moonLit - repeated.moonLit) <= 0.02f);
    }
}