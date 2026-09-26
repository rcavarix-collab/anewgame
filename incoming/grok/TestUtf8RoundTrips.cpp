static void TestUtf8RoundTrips()
{
    // UTF-8 length boundaries and representative interior code points.
    const uint32_t codePoints[] = {
        0x7F,
        0x80,
        0x123,
        0x7FF,
        0x800,
        0x1234,
        0xFFFF,
        0x10000,
        0x12345,
        0x10FFFF
    };

    for (uint32_t cp : codePoints)
    {
        std::string encoded;
        AppendUtf8(encoded, cp);

        size_t i = 0;
        const uint32_t decoded = DecodeUtf8(encoded, i);

        CHECK(decoded == cp);
        CHECK(i == encoded.size());
    }

    // Overlong encoding of ASCII 'A' (U+0041).
    {
        const std::string invalid[] = {
            std::string("\xC0\x81", 2),
            std::string("\xE0\x80\x81", 3),
            std::string("\xF0\x80\x80\x81", 4)
        };

        for (const std::string& s : invalid)
        {
            size_t i = 0;
            CHECK(DecodeUtf8(s, i) == 0xFFFD);
            CHECK(i == 1);
        }
    }

    // Truncated two-, three-, and four-byte sequences.
    {
        const std::string invalid[] = {
            std::string("\xC2", 1),
            std::string("\xE2\x82", 2),
            std::string("\xF0\x9F\x92", 3)
        };

        for (const std::string& s : invalid)
        {
            size_t i = 0;
            CHECK(DecodeUtf8(s, i) == 0xFFFD);
            CHECK(i == 1);
        }
    }
}