static void TestGameFolderChoice()
{
    namespace fs = std::filesystem;

    const fs::path documents = fs::path("C:/Users/Player/Documents");
    const fs::path savedGames = fs::path("C:/Users/Player/Saved Games");

    // 1. Ordinary Documents path.
    {
        const fs::path actual = ChooseGameFolder(documents, savedGames);
        CHECK(actual == documents / "My Games" / "walkgrid");
    }

    // 2. OneDrive component with exact casing.
    {
        const fs::path docs = fs::path("C:/Users/Player/OneDrive/Documents");
        const fs::path actual = ChooseGameFolder(docs, savedGames);
        CHECK(actual == savedGames / "walkgrid");
    }

    // 3. Mixed-case OneDrive component.
    {
        const fs::path docs = fs::path("C:/Users/Player/oNeDrIvE/Documents");
        const fs::path actual = ChooseGameFolder(docs, savedGames);
        CHECK(actual == savedGames / "walkgrid");
    }

    // 4. OneDriveBackup is not an exact OneDrive component.
    {
        const fs::path docs = fs::path("C:/Users/Player/OneDriveBackup/Documents");
        const fs::path actual = ChooseGameFolder(docs, savedGames);
        CHECK(actual == docs / "My Games" / "walkgrid");
    }

    // 5. Empty Documents path uses Saved Games.
    {
        const fs::path actual = ChooseGameFolder(fs::path{}, savedGames);
        CHECK(actual == savedGames / "walkgrid");
    }

    // 6. Both paths empty returns an empty path.
    {
        const fs::path actual = ChooseGameFolder(fs::path{}, fs::path{});
        CHECK(actual.empty());
    }

    // 7. OneDrive Documents with empty Saved Games returns an empty path.
    {
        const fs::path docs = fs::path("C:/Users/Player/OneDrive/Documents");
        const fs::path actual = ChooseGameFolder(docs, fs::path{});
        CHECK(actual.empty());
    }

    // 8. Forward-slash path input.
    {
        const fs::path docs = fs::path("C:/Users/Player/OneDrive/Documents");
        const fs::path actual = ChooseGameFolder(docs, savedGames);
        CHECK(actual == savedGames / "walkgrid");
    }

    // 9. Backslash path input (Windows-native path semantics).
    {
        const fs::path docs = fs::path("C:\\Users\\Player\\OneDrive\\Documents");
        const fs::path actual = ChooseGameFolder(docs, savedGames);
        CHECK(actual == savedGames / "walkgrid");
    }
}