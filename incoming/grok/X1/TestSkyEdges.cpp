static void TestSkyEdges() {
    CHECK(DiscCover(SUN_DISC_RADIUS, MOON_DISC_RADIUS, 1.0f) == 0.0f);
    CHECK(DiscCover(MOON_DISC_RADIUS, SUN_DISC_RADIUS, 1.0f) == 0.0f);
    CHECK(DiscCover(0.1f, 0.05f, 0.2f) == 0.0f);
    CHECK(DiscCover(0.01f, 0.1f, 0.0f) == 1.0f);
    CHECK(DiscCover(0.01f, 0.1f, 0.05f) == 1.0f);
    CHECK(DiscCover(SUN_DISC_RADIUS * 0.5f, SUN_DISC_RADIUS, 0.0f) == 1.0f);
    {
        const float r1 = SUN_DISC_RADIUS;
        const float r2 = MOON_DISC_RADIUS;
        float prev = DiscCover(r1, r2, 0.0f);
        for (int i = 1; i <= 200; ++i) {
            const float d = static_cast<float>(i) * 1e-4f;
            const float cur = DiscCover(r1, r2, d);
            const float delta = cur > prev ? (cur - prev) : (prev - cur);
            CHECK(delta < 0.05f);
            prev = cur;
        }
    }
    {
        bool foundNear0 = false;
        bool foundNear1 = false;
        for (uint32_t day = 0; day <= 7; ++day) {
            for (int t = 0; t < 3600; t += 60) {
                const SkyState s = ComputeSky(static_cast<float>(t), day);
                if (s.moonLit < 0.05f) foundNear0 = true;
                if (s.moonLit > 0.95f) foundNear1 = true;
            }
        }
        CHECK(foundNear0);
        CHECK(foundNear1);
    }
    {
        for (uint32_t day = 0; day < 30; ++day) {
            const double T_end = static_cast<double>(day) + 3599.0 / 3600.0;
            const double T_next = static_cast<double>(day + 1);
            const float a0 = JetStreamAngle(T_end);
            const float a1 = JetStreamAngle(T_next);
            float diff = a1 - a0;
            if (diff < 0.0f) diff = -diff;
            if (diff > 3.14159265f) diff = 6.2831853f - diff;
            CHECK(diff < 1e-4f);
        }
    }
    {
        bool foundEclipse = false;
        float eclipseDayTime = 0.0f;
        uint32_t eclipseDay = 0;
        for (uint32_t day = 0; day <= 400 && !foundEclipse; ++day) {
            for (int t = 1500; t <= 2100; t += 30) {
                const SkyState s = ComputeSky(static_cast<float>(t), day);
                if (s.solarEclipse >= 0.99f) {
                    foundEclipse = true;
                    eclipseDayTime = static_cast<float>(t);
                    eclipseDay = day;
                    break;
                }
            }
        }
        CHECK(foundEclipse);
        if (foundEclipse) {
            const SkyState s = ComputeSky(eclipseDayTime, eclipseDay);
            const Atmosphere a = ComputeAtmosphere(s);
            CHECK(a.sunColor.x < 0.02f);
            CHECK(a.sunColor.y < 0.02f);
            CHECK(a.sunColor.z < 0.02f);
        }
    }
}
