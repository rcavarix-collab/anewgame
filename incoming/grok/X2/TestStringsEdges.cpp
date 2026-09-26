static void TestStringsEdges()
{
    std::unordered_map<std::string, std::string> table;
    std::vector<std::string> errors;

    // key defined twice (later wins)
    table.clear();
    errors.clear();
    ParseStrings("k = first\nk = second", "twice.txt", table, errors);
    CHECK(errors.empty());
    CHECK(table.count("k") == 1u);
    CHECK(table["k"] == "second");

    // line without '='
    table.clear();
    errors.clear();
    ParseStrings("noequalsline\nok = value", "badline.txt", table, errors);
    CHECK(!errors.empty());
    CHECK(table["ok"] == "value");

    // 10 000-character text
    std::string longText(10000, 'x');
    table.clear();
    errors.clear();
    ParseStrings("long = " + longText, "long.txt", table, errors);
    CHECK(errors.empty());
    CHECK(table["long"] == longText);
    CHECK(table["long"].size() == 10000u);

    // {9} with fewer arguments
    SetStrings({{"fmt", "prefix {9} suffix"}});
    CHECK(StrF("fmt", {"only0"}) == "prefix {9} suffix");

    // invalid UTF-8 mid-line (via DecodeUtf8)
    {
        std::string s = "ab\x80cd";
        size_t i = 0;
        CHECK(DecodeUtf8(s, i) == static_cast<uint32_t>('a'));
        CHECK(i == 1u);
        CHECK(DecodeUtf8(s, i) == static_cast<uint32_t>('b'));
        CHECK(i == 2u);
        CHECK(DecodeUtf8(s, i) == 0xFFFDu);
        CHECK(i == 3u);
        CHECK(DecodeUtf8(s, i) == static_cast<uint32_t>('c'));
        CHECK(i == 4u);
        CHECK(DecodeUtf8(s, i) == static_cast<uint32_t>('d'));
        CHECK(i == 5u);
    }

    // WideToUtf8(Utf8ToWide(s)) == s for three accented strings
    {
        const char* samples[] = {
            "caf\xc3\xa9",          // café
            "na\xc3\xafve",         // naïve
            "r\xc3\xa9sum\xc3\xa9"  // résumé
        };
        for (const char* p : samples) {
            std::string s(p);
            CHECK(WideToUtf8(Utf8ToWide(s)) == s);
        }
    }

    SetStrings({});
}
