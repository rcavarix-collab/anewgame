static void TestStringsMore()
{
    SetStrings({
        {"reverse", "{2}|{1}|{0}"},
        {"repeat", "{1}-{0}-{1}"},
        {"present", "value"},
        {"accent_a", "café"},
        {"accent_b", "élève"},
        {"font", "ignored Ω"}
    });

    CHECK(StrF("reverse", {"A", "B", "C"}) == "C|B|A");
    CHECK(StrF("repeat", {"first", "second"}) == "second-first-second");

    CHECK(HasStr("present"));
    CHECK(!HasStr("missing"));

    const std::vector<uint32_t> codepoints = StringCodepoints();

    CHECK(std::is_sorted(codepoints.begin(), codepoints.end()));
    CHECK(std::adjacent_find(codepoints.begin(), codepoints.end()) == codepoints.end());

    const std::vector<uint32_t> expected = {
        0x20u, 0x2Du, 0x61u, 0x63u, 0x65u, 0x6Cu,
        0x72u, 0x76u, 0xE8u, 0xE9u
    };

    CHECK(codepoints == expected);

    SetStrings({});
}