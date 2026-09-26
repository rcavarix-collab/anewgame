
#include <windows.h>   // Header for Windows API, provides access to Windows-specific functions and data types.
#include <vector>      // Standard vector library for dynamic arrays, allows for efficient storage and manipulation of elements.
#include <fstream>     // File input/output library, enables reading from and writing to files.
#include <string>      // String manipulation library, provides support for handling and manipulating strings.
#include <shlobj.h>    // Shell functions for folder paths (e.g., saving/loading game), provides access to shell utilities.
#include <locale>      // Locale-related functions for character conversion, enables localization and text formatting.
#include <codecvt>     // For converting between wide and UTF-8 strings, facilitates encoding conversions.
#include <set>         // Used for tracking unique positions, provides an associative container for unique elements.
#include <utility>     // Provides utility functions and classes, such as std::pair and std::swap.
#include <gdiplus.h>   // For GDI+ Graphics, supports advanced 2D graphics, including rendering and image processing.
#include <cmath>       // For mathematical functions, including trigonometric functions like sin.
#include <unordered_map> // Provides a hash table-based implementation for key-value pairs, allowing for fast lookups.
#include <functional>  // Defines function objects and standard function wrappers, used for callbacks and higher-order functions.

COLORREF BACKGROUND_COLOR = RGB(101, 67, 33);       // Brown background color




//NEWcode for controlling menu override state via bool state
bool isMenuOpen = false;
bool isFullscreenMenuOpen = false;



using namespace Gdiplus;

ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;
void CleanupSprites();

Bitmap* checkerboard_pattern_1;
Bitmap* chevron_pattern_1;
Bitmap* concentricdiamond_pattern_1;
Bitmap* denseplaid_pattern_1;
Bitmap* diagonalgrid_pattern_1;
Bitmap* diagonalherringbone_pattern_1;
Bitmap* diagonalzigzag_pattern_1;
Bitmap* diamond_pattern_1;
Bitmap* dottedhalftone_pattern_1;
Bitmap* flower_pattern_1;
Bitmap* flowervar1_pattern_1;
Bitmap* flowervar2_pattern_1;
Bitmap* flowervar3_pattern_1;
Bitmap* gears_pattern_1;
Bitmap* herringbone_pattern_1;
Bitmap* offsetdiagonallines_pattern_1;
Bitmap* offsetherringbone_pattern_1;
Bitmap* plaid_pattern_1;
Bitmap* smoothzigzag_pattern_1;
Bitmap* vstripe_pattern_1;
Bitmap* zigzagherringbone_pattern_1;
Bitmap* zigzagstripes_pattern_1;
Bitmap* linkedrings_pattern_1;
Bitmap* flowervar12_pattern_1;
Bitmap* flowervar14_pattern_1;
Bitmap* concentricflowers_pattern_1;
Bitmap* flowerwave_pattern_1;
Bitmap* flowerdiamond_pattern_1;
Bitmap* honeycomb_pattern_1;
Bitmap* linkedsquares_pattern_1;
Bitmap* radialzigzag_pattern_1;
Bitmap* fractalhalftone_pattern_1;
Bitmap* red_solid_pattern;
Bitmap* x_text_pattern;
Bitmap* hybridorganic_pattern;
Bitmap* gearswithcuts_pattern;
Bitmap* gearswithspokes_pattern;
Bitmap* gearsandchevrons_pattern;
Bitmap* diagonalchevron_pattern;
Bitmap* herringbonechevron_pattern;



const wchar_t CLASS_NAME[] = L"LGR"; // Window class name for registration
const int GRID_SIZE = 32;                       // Size of each grid cell (pixels)
const int GRID_COLS_INITIAL = 32;              // Initial number of grid columns
const int GRID_ROWS_INITIAL = 32;              // Initial number of grid rows
int GRID_COLS = GRID_COLS_INITIAL;              // Current number of columns
int GRID_ROWS = GRID_ROWS_INITIAL;              // Current number of rows
int WIDTH = GRID_COLS * GRID_SIZE;              // Width of the game window in pixels
int HEIGHT = GRID_ROWS * GRID_SIZE;             // Height of the game window in pixels

std::vector<std::vector<bool>> grid(GRID_ROWS, std::vector<bool>(GRID_COLS, false));
struct Block {
    int x;
    int y;
    int type;
    // add other block aspects here

        // Default constructor with member initializer list
    Block(int x = 0, int y = 0, int type = 0)
        : x(x), y(y), type(type) {
    }
};

std::vector<Block> savedBlocks;  // Store overwritten blocks

std::vector<Block> savedFullscreenBlocks;  // Store game state when menu is open



// Global variables for reusable brushes and device context
HDC hdcMem;
HBITMAP hbmMem;
HBITMAP hbmOld;
HBRUSH backgroundBrush;

void InitializeBrushes() {
    // Create all brushes once
    backgroundBrush = CreateSolidBrush(BACKGROUND_COLOR);

}

void InitializeMemoryDC(HDC hdc) {
    if (!hdcMem) {  // Initialize once
        hdcMem = CreateCompatibleDC(hdc); // Create a compatible memory device context
        hbmMem = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT); // Create a bitmap to hold the backbuffer
        hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem); // Select the bitmap into the memory DC
    }
}

void CleanUp() {
    // Delete brush(es) when the game closes
    DeleteObject(backgroundBrush);

    if (hdcMem) {
        SelectObject(hdcMem, hbmOld); // Restore the original bitmap
        DeleteObject(hbmMem); // Delete the compatible bitmap
        DeleteDC(hdcMem); // Delete the memory device context
        hdcMem = nullptr; // Reset pointer to avoid accidental reuse
    }
}

std::vector<Block> blocks; // List of blocks in the game
int currentBlockType = 0;
void DrawScene(HDC hdc);                 // Draws the game scene
void LoadGame();                         // Loads the game state from a file
void SaveGame();                         // Saves the game state to a file
std::wstring GetSaveFilePath();          // Gets the file path for saving/loading


std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], sizeNeeded, nullptr, nullptr);
    result.pop_back(); // Remove the null terminator
    return result;
}

std::wstring stringToWstring(const std::string& str) {
    if (str.empty()) return L"";
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], sizeNeeded);
    result.pop_back(); // Remove the null terminator
    return result;
}

void SaveGame() {
    std::wstring filePath = GetSaveFilePath();  // Get save file path
    std::ofstream saveFile(filePath, std::ios::binary); // Open file in binary write mode

    if (saveFile.is_open()) {


        // Save number of blocks
        size_t blockCount = blocks.size();
        saveFile.write(reinterpret_cast<const char*>(&blockCount), sizeof(blockCount));

        // Save each block's data
        for (const Block& block : blocks) {
            saveFile.write(reinterpret_cast<const char*>(&block.x), sizeof(block.x));
            saveFile.write(reinterpret_cast<const char*>(&block.y), sizeof(block.y));
            saveFile.write(reinterpret_cast<const char*>(&block.type), sizeof(block.type));
        }

        saveFile.close(); // Close the file
    }
    else {
        OutputDebugStringW((L"Failed to save game to " + filePath + L"\n").c_str()); // Error handling
    }
}

void LoadGame() {
    std::wstring filePath = GetSaveFilePath();  // Get save file path
    std::ifstream loadFile(filePath, std::ios::binary); // Open file in binary read mode

    if (loadFile.is_open()) {
        blocks.clear(); // Clear existing blocks


        // Load number of blocks
        size_t blockCount;
        loadFile.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));

        // Load each block's data
        for (size_t i = 0; i < blockCount; ++i) {
            Block block;
            loadFile.read(reinterpret_cast<char*>(&block.x), sizeof(block.x));
            loadFile.read(reinterpret_cast<char*>(&block.y), sizeof(block.y));
            loadFile.read(reinterpret_cast<char*>(&block.type), sizeof(block.type));

            blocks.push_back(block); // Add block to the list
        }

        loadFile.close(); // Close the file
    }
    else {
        OutputDebugStringW((L"Failed to load game from " + filePath + L"\n").c_str()); // Error handling
    }
}

bool IsBlockOfType(int x, int y, int type) {
    for (const auto& block : blocks) {
        if (block.x == x && block.y == y) {
            return block.type == type;
        }
    }
    return false;
}

void RemoveBlockAtPosition(int x, int y) {
    // Find the block at the given (x, y) and remove it
    auto it = std::remove_if(blocks.begin(), blocks.end(), [x, y](const Block& block) {
        return block.x == x && block.y == y;
        });
    // Erase the removed block(s) from the vector
    if (it != blocks.end()) {
        blocks.erase(it, blocks.end());
    }
}

bool IsBlockAtPosition(int x, int y) {
    for (const Block& block : blocks) {
        if (block.x == x && block.y == y) {
            return true; // Return true if block exists
        }
    }
    return false;
}

bool IsBlockOfTypeAround(int x, int y, int type) {
    int directions[8][2] = {
        {0, -GRID_SIZE}, {0, GRID_SIZE}, {-GRID_SIZE, 0}, {GRID_SIZE, 0}, // Up, Down, Left, Right
        {-GRID_SIZE, -GRID_SIZE}, {GRID_SIZE, GRID_SIZE}, {-GRID_SIZE, GRID_SIZE}, {GRID_SIZE, -GRID_SIZE} // Diagonals
    };

    for (int i = 0; i < 8; ++i) {
        int newX = x + directions[i][0];
        int newY = y + directions[i][1];
        if (IsBlockOfType(newX, newY, type)) {
            return true; // If a block of the given type is found nearby, return true
        }
    }
    return false; // No blocks of the given type found nearby
}

void InitializeScene() {
    if (!blocks.empty()) return;  // Keep saved game

    blocks.clear();

    const int TILE = GRID_SIZE;        // 32
    const int PREVIEW = 3;                // 3×3 tiles per pattern
    const int BLOCK = PREVIEW * TILE;   // 96×96 px per pattern block
    const int GAP = 28;               // space between blocks (tight but clean)

    // 8 × 5 = 40 patterns exactly
    const int COLS = 8;
    const int ROWS = 5;

    int totalWidth = COLS * BLOCK + (COLS - 1) * GAP;
    int totalHeight = ROWS * BLOCK + (ROWS - 1) * GAP;

    // Center the entire 8×5 grid
    int startX = (WIDTH - totalWidth) / 2;
    int startY = (HEIGHT - totalHeight) / 2;

    // Prevent negative offset (in case window is resized smaller)
    if (startX < 16) startX = 16;
    if (startY < 16) startY = 16;

    int patternType = 1;

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (patternType > 40) break;

            int baseX = startX + col * (BLOCK + GAP);
            int baseY = startY + row * (BLOCK + GAP);

            // Create one solid 3×3 block (9 tiles)
            for (int dy = 0; dy < PREVIEW; ++dy) {
                for (int dx = 0; dx < PREVIEW; ++dx) {
                    int x = baseX + dx * TILE;
                    int y = baseY + dy * TILE;

                    // Only place if inside window (safety)
                    if (x + TILE <= WIDTH && y + TILE <= HEIGHT) {
                        blocks.emplace_back(x, y, patternType);
                    }
                }
            }

            patternType++;
        }
    }
}



void OpenMenu() {
    if (!isMenuOpen) {
        isMenuOpen = true;
        savedBlocks.clear();

        // Save all existing blocks
        savedBlocks = blocks;

        // Clear the entire grid
        blocks.clear();

        // Fill the entire grid with menu blocks
        for (int y = 0; y < GRID_ROWS; ++y) {
            for (int x = 0; x < GRID_COLS; ++x) {
                blocks.push_back(Block(x * GRID_SIZE, y * GRID_SIZE, 1));  // Example: menu uses block type 1
            }
        }
    }
}



void CloseMenu() {
    if (isMenuOpen) {
        isMenuOpen = false;

        // Restore original blocks
        blocks = savedBlocks;
    }
}


void OpenFullscreenMenu() {
    if (!isFullscreenMenuOpen) {
        isFullscreenMenuOpen = true;
        savedFullscreenBlocks = blocks; // Save the current game state
        blocks.clear(); // Clear the grid for the fullscreen menu

        // Fill the screen with menu elements (Example: block type 99)
        for (int y = 0; y < GRID_ROWS; ++y) {
            for (int x = 0; x < GRID_COLS; ++x) {
                blocks.push_back(Block(x * GRID_SIZE, y * GRID_SIZE, 99));  // Example: menu uses block type 99
            }
        }
        InvalidateRect(nullptr, NULL, TRUE); // Refresh window
    }
}
void CloseFullscreenMenu() {
    if (isFullscreenMenuOpen) {
        isFullscreenMenuOpen = false;
        blocks = savedFullscreenBlocks; // Restore game state
        InvalidateRect(nullptr, NULL, TRUE); // Refresh window
    }
}








/////^^^^NEWISH


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        HDC hdc = GetDC(hwnd);
        InitializeMemoryDC(hdc);  // Initialize the memory DC once here
        ReleaseDC(hwnd, hdc);
        LoadGame();  // Load saved game
        InitializeScene();  // Only applies if no save file exists
        return 0;
    }
    case WM_DESTROY: {// When the window is closed
        if (isMenuOpen) {
            CloseMenu(); // Restore game state before exiting
        }
        if (isFullscreenMenuOpen) {
            CloseFullscreenMenu(); // Restore game state before exiting
        }
        SaveGame();
        CleanUp();// Save the game
        CleanupSprites();
        PostQuitMessage(0); // End the program
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }
    case WM_PAINT: { // Repainting the window
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawScene(hdc); // Draw the game scene
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_RBUTTONDOWN: {
        int x = LOWORD(lParam) / GRID_SIZE * GRID_SIZE;
        int y = HIWORD(lParam) / GRID_SIZE * GRID_SIZE;
        RemoveBlockAtPosition(x, y); // Remove block at the clicked position
        InvalidateRect(hwnd, NULL, TRUE); // Refresh window
        return 0;
    }
    case WM_KEYDOWN: {// KEY CONTROL INPUTS

        switch (wParam) {


        case '1': currentBlockType = 1; break;
        case '2': currentBlockType = 2; break;
        case '3': currentBlockType = 3; break; // broken
        case '4': currentBlockType = 4; break;
        case '5': currentBlockType = 5; break;
        case '6': currentBlockType = 6; break;
        case '7': currentBlockType = 7; break;
        case '8': currentBlockType = 8; break;
        case '9': currentBlockType = 9; break;
        case '0': currentBlockType = 10; break; //needs more spin to sprial

        case 'Q': currentBlockType = 11; break;
        case 'W': currentBlockType = 12; break; // could be more dense/complex
        case 'E': currentBlockType = 13; break; //dup alt of 12
        case 'R': currentBlockType = 14; break;// could be more dense/complex
        case 'T': currentBlockType = 15; break;//dupe alt of above
        case 'Y': currentBlockType = 16; break;// could be more dense/complex
        case 'U': currentBlockType = 17; break;//dupe alt of above
        case 'I': currentBlockType = 18; break;// could be more dense/complex
        case 'O': currentBlockType = 19; break;//dupe alt of above
        case 'P': currentBlockType = 20; break;// could be more dense/complex

        case 'A': currentBlockType = 21; break;
        case 'S': currentBlockType = 22; break;//broken
        case 'D': currentBlockType = 23; break;
        case 'F': currentBlockType = 24; break; //dupe alt of above
        case 'G': currentBlockType = 25; break;//dupe alt of above
        case 'H': currentBlockType = 26; break;
        case 'J': currentBlockType = 27; break;
        case 'K': currentBlockType = 28; break;
        case 'L': currentBlockType = 29; break;
        case VK_OEM_1:    // Maps to both ';' and ':' keys
            currentBlockType = 30;
            break;

        case 'Z': currentBlockType = 31; break;
        case 'X': currentBlockType = 32; break;//optical illusiony
        case 'C': currentBlockType = 33; break;//too blurry
        case 'V': currentBlockType = 34; break;
        case 'B': currentBlockType = 35; break;
        case 'N': currentBlockType = 36; break;// could be more dense/complex
        case 'M': currentBlockType = 37; break;
        case VK_OEM_COMMA:    // Maps to both ',' and '<' keys
            currentBlockType = 38;
            break;

        case VK_OEM_PERIOD:   // Maps to both '.' and '>' keys
            currentBlockType = 39;
            break;

        case VK_OEM_2:    // Maps to both '/' and '?' keys
            currentBlockType = 40;
            break;

        case VK_OEM_3:  // The tilde (`~`) key
            if (isFullscreenMenuOpen) {
                CloseFullscreenMenu();
            }
            else {
                OpenFullscreenMenu();
            }
            return 0;


        case VK_ESCAPE:
            if (isMenuOpen) {
                CloseMenu();
            }
            else {
                OpenMenu();
            }
            InvalidateRect(hwnd, NULL, TRUE); // Refresh window
            return 0;

        }
        InvalidateRect(hwnd, NULL, FALSE); // Refresh window-- 11 and beyond unlisted intentionally
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam) / GRID_SIZE * GRID_SIZE;
        int y = HIWORD(lParam) / GRID_SIZE * GRID_SIZE;

        // Remove any block that may already exist at the clicked position
        RemoveBlockAtPosition(x, y);

        // Create a new block at the clicked position with the current block type
        Block newBlock = { x, y, currentBlockType };





        // Add the new block to the game's block list
        blocks.push_back(newBlock);
        InvalidateRect(hwnd, NULL, TRUE);  // Refresh the window to show the new block
        return 0;
    }

    case WM_TIMER:
        if (!isMenuOpen) {
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default message handling
}



Bitmap* CreateCheckerboardPattern(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    // Fill the bitmap with a checkerboard pattern
    for (int y = 0; y < height; y += GRID_SIZE / 4) { // Change to GRID_SIZE / 4
        for (int x = 0; x < width; x += GRID_SIZE / 4) { // Change to GRID_SIZE / 4
            if ((x / (GRID_SIZE / 4) + y / (GRID_SIZE / 4)) % 2 == 0) { // Change to GRID_SIZE / 4
                SolidBrush brush(color1); // Use the first color
                graphics.FillRectangle(&brush, x, y, GRID_SIZE / 4, GRID_SIZE / 4); // Change to GRID_SIZE / 4
            }
            else {
                SolidBrush brush(color2); // Use the second color
                graphics.FillRectangle(&brush, x, y, GRID_SIZE / 4, GRID_SIZE / 4); // Change to GRID_SIZE / 4
            }
        }
    }

    return bitmap;
}



Bitmap* CreateFlowerPattern(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    // Fill with background color
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    // Define flower pattern
    SolidBrush flowerBrush(flowerColor);

    // Adjust pattern so it fits a 32x32 workspace and tessellates seamlessly
    int patternSize = 8; // Define the size of the flower/star pattern that fits within the 32x32 grid
    int flowerRadius = patternSize / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create a repeating flower pattern based on modulo math to ensure tessellation
            int modX = x % patternSize;
            int modY = y % patternSize;

            // Create a radial or cross-like flower pattern within the pattern size
            // Symmetric flower/star shapes, centered in each tile block
            if ((modX == flowerRadius || modY == flowerRadius) ||
                (modX % flowerRadius == 0 && modY % flowerRadius == 0)) {
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1); // Fill the pixel for the flower/star shape
            }
        }
    }

    return bitmap;
}




Bitmap* CreatePlaidPattern(int width, int height, const Color& stripeColor1, const Color& stripeColor2, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int stripeWidth = GRID_SIZE / 4;

    // Draw vertical stripes
    SolidBrush brush1(stripeColor1);
    for (int x = 0; x < width; x += stripeWidth * 2) {
        graphics.FillRectangle(&brush1, x, 0, stripeWidth, height);
        graphics.FillRectangle(&brush1, x + stripeWidth / 2, 0, stripeWidth / 2, height);
    }

    // Draw horizontal stripes
    SolidBrush brush2(stripeColor2);
    for (int y = 0; y < height; y += stripeWidth * 2) {
        graphics.FillRectangle(&brush2, 0, y, width, stripeWidth);
        graphics.FillRectangle(&brush2, 0, y + stripeWidth / 2, width, stripeWidth / 2);
    }

    return bitmap;
}

Bitmap* CreateHerringbonePattern(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int brickWidth = GRID_SIZE / 3;
    int brickHeight = GRID_SIZE / 6;

    for (int y = 0; y < height; y += brickHeight) {
        for (int x = (y / brickHeight) % 2 == 0 ? 0 : -brickWidth / 2; x < width + brickWidth; x += brickWidth) {
            SolidBrush brush((x / brickWidth + y / brickHeight) % 2 == 0 ? color1 : color2);
            graphics.FillRectangle(&brush, x, y, brickWidth / 2, brickHeight);
            graphics.FillRectangle(&brush, x + brickWidth / 2, y + brickHeight / 2, brickWidth / 2, brickHeight);
        }
    }

    return bitmap;
}






Bitmap* CreateDiamondPattern(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int diamondSize = 4;
    for (int y = 0; y < height; y += diamondSize) {
        for (int x = 0; x < width; x += diamondSize) {
            SolidBrush brush(((x / diamondSize) + (y / diamondSize)) % 2 == 0 ? color1 : color2);

            PointF points[4] = {
                PointF(static_cast<Gdiplus::REAL>(x + diamondSize / 2), static_cast<Gdiplus::REAL>(y)),          // Top
                PointF(static_cast<Gdiplus::REAL>(x + diamondSize), static_cast<Gdiplus::REAL>(y + diamondSize / 2)), // Right
                PointF(static_cast<Gdiplus::REAL>(x + diamondSize / 2), static_cast<Gdiplus::REAL>(y + diamondSize)), // Bottom
                PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y + diamondSize / 2))          // Left
            };
            graphics.FillPolygon(&brush, points, 4);
        }
    }

    return bitmap;
}

Bitmap* GenerateDiagonalGrid(int width, int height, const Color& lineColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    Pen pen(lineColor, 1);

    // Adjust line step size and limits for tessellation in a 32x32 grid
    int stepSize = 4; // Smaller step size to fit within a 32x32 grid
    int offset = width % stepSize; // Calculate offset for proper tiling

    // Draw diagonal lines in both directions
    for (int i = -height; i < width + height; i += stepSize) {
        // Draw top-left to bottom-right diagonals
        graphics.DrawLine(&pen, Point(i, 0), Point(i + height - offset, height));
        // Draw top-right to bottom-left diagonals
        graphics.DrawLine(&pen, Point(0, i), Point(width, i + width - offset));
    }

    return bitmap;
}





Bitmap* CreateChevronPattern(int width, int height, const Color& primaryColor, const Color& secondaryColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    // Size adjustments to fit within 32x32
    int chevronWidth = width / 4; // Adjusting to fit more evenly within the grid
    int chevronHeight = height / 4;

    // Iterate over the grid to fill with chevrons
    for (int y = 0; y < height; y += chevronHeight) {
        for (int x = 0; x < width; x += chevronWidth) {
            SolidBrush brush((x / chevronWidth + y / chevronHeight) % 2 == 0 ? primaryColor : secondaryColor);

            // Adjust chevron points to ensure they align well and tessellate
            Point points[3] = {
                Point(x, y),
                Point(x + chevronWidth / 2, y + chevronHeight),
                Point(x + chevronWidth, y)
            };

            graphics.FillPolygon(&brush, points, 3);
        }
    }

    return bitmap;
}





Bitmap* CreateDiagonalZigzagPattern(int width, int height, const Color& lineColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    Pen pen(lineColor, 2);  // A thinner pen for clarity in a smaller tile

    // Adjust the zigzag pattern width and height for the 32x32 grid
    int zigzagWidth = width / 4;  // Adjusted to fit better in the 32x32 grid
    int zigzagHeight = height / 4;  // Adjusted for a more balanced pattern

    // Generate the zigzag pattern within the constraints of the tile
    for (int y = 0; y < height; y += zigzagHeight) {
        for (int x = 0; x < width; x += zigzagWidth) {
            // Coordinates for the zigzag points, ensuring the pattern fits
            PointF points[4] = {
                PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y)),
                PointF(static_cast<Gdiplus::REAL>(x + zigzagWidth / 2), static_cast<Gdiplus::REAL>(y + zigzagHeight)),
                PointF(static_cast<Gdiplus::REAL>(x + zigzagWidth), static_cast<Gdiplus::REAL>(y)),
                PointF(static_cast<Gdiplus::REAL>(x + zigzagWidth * 1.5f), static_cast<Gdiplus::REAL>(y + zigzagHeight))
            };
            graphics.DrawLines(&pen, points, 4);
        }
    }

    return bitmap;
}



Bitmap* CreateVStripePattern(int width, int height, const Color& primaryColor, const Color& secondaryColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int stripeWidth = width / 10;

    for (int x = 0; x < width; x += stripeWidth) {
        SolidBrush brush((x / stripeWidth) % 2 == 0 ? primaryColor : secondaryColor);
        graphics.FillRectangle(&brush, x, 0, stripeWidth, height);
    }
    return bitmap;
}

Bitmap* CreateVerticalStripePattern(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int stripeWidth = GRID_SIZE / 3;

    for (int x = 0; x < width; x += stripeWidth * 2) {
        SolidBrush brush1(color1);
        SolidBrush brush2(color2);

        // First stripe
        graphics.FillRectangle(&brush1, x, 0, stripeWidth, height);

        // Second stripe
        graphics.FillRectangle(&brush2, x + stripeWidth, 0, stripeWidth, height);
    }

    return bitmap;
}

Bitmap* CreateZigZagHerringbone(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int brickWidth = GRID_SIZE / 4;
    int brickHeight = GRID_SIZE / 6;

    for (int y = 0; y < height; y += brickHeight) {
        for (int x = (y / brickHeight) % 2 == 0 ? 0 : -brickWidth / 2; x < width + brickWidth; x += brickWidth) {
            SolidBrush brush((x / brickWidth + y / brickHeight) % 2 == 0 ? color1 : color2);
            graphics.FillRectangle(&brush, x, y, brickWidth, brickHeight / 2);
            graphics.FillRectangle(&brush, x + brickWidth / 2, y + brickHeight / 2, brickWidth, brickHeight / 2);
        }
    }

    return bitmap;
}

Bitmap* CreateDiagonalHerringbone(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int brickWidth = GRID_SIZE / 3;
    int brickHeight = GRID_SIZE / 5;

    for (int y = 0; y < height; y += brickHeight) {
        for (int x = (y / brickHeight) % 2 == 0 ? 0 : -brickWidth / 2; x < width + brickWidth; x += brickWidth) {
            SolidBrush brush((x / brickWidth + y / brickHeight) % 2 == 0 ? color1 : color2);
            graphics.FillRectangle(&brush, x, y, brickWidth / 2, brickHeight / 2);
            graphics.FillRectangle(&brush, x + brickWidth / 2, y + brickHeight / 2, brickWidth / 2, brickHeight / 2);
        }
    }

    return bitmap;
}

Bitmap* CreateOffsetHerringbone(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int brickWidth = GRID_SIZE / 2;
    int brickHeight = GRID_SIZE / 6;

    for (int y = 0; y < height; y += brickHeight) {
        for (int x = (y / brickHeight) % 2 == 0 ? 0 : -brickWidth / 3; x < width + brickWidth; x += brickWidth) {
            SolidBrush brush((x / brickWidth + y / brickHeight) % 2 == 0 ? color1 : color2);
            graphics.FillRectangle(&brush, x, y, brickWidth / 3, brickHeight);
            graphics.FillRectangle(&brush, x + brickWidth / 3, y + brickHeight / 3, brickWidth / 3, brickHeight);
        }
    }

    return bitmap;
}

Bitmap* CreateOffsetDiagonalLines(int width, int height, int spacing, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    for (int y = 0; y < height; y += spacing) {
        for (int x = 0; x < width; x += spacing) {
            int offsetX = (y / spacing) % 2 == 0 ? 0 : spacing / 2;
            SolidBrush brush((x + offsetX) % (2 * spacing) < spacing ? color1 : color2);
            graphics.FillRectangle(&brush, x + offsetX, y, spacing, spacing);
        }
    }

    return bitmap;
}

Bitmap* CreateZigzagStripes(int width, int height, int stripeWidth, int amplitude, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int zigzagX = x + static_cast<int>(sin(y / static_cast<float>(stripeWidth)) * amplitude);
            SolidBrush brush((zigzagX / stripeWidth) % 2 == 0 ? color1 : color2);
            graphics.FillRectangle(&brush, x, y, 1, 1);
        }
    }

    return bitmap;
}

Bitmap* CreateConcentricDiamond(int width, int height, float thickness, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float dx = fabsf(x - centerX);
            float dy = fabsf(y - centerY);
            int ring = static_cast<int>((dx + dy) / thickness);

            SolidBrush brush((ring % 2 == 0) ? color1 : color2);
            graphics.FillRectangle(&brush, x, y, 1, 1);
        }
    }

    return bitmap;
}

Bitmap* CreateFlowerVariation1(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);

    // Adjust the pattern to fit better within the 32x32 pixel tile
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Tiling pattern that works at the borders and within the grid
            if ((x % 8 == 0 && y % 8 == 0) || (x % 8 == 4 && y % 8 == 4) || (x % 8 == 4 && y % 8 == 0) || (x % 8 == 0 && y % 8 == 4)) {
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }

    return bitmap;
}


Bitmap* CreateFlowerVariation2(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (((x + y) % 4 == 0) || ((x - y + width) % 4 == 0)) { // Use (x - y + width) to avoid negative modulo
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }
    return bitmap;
}

Bitmap* CreateFlowerVariation3(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);

    // Adjust the pattern size to fit within the 32x32 grid for tessellation
    int patternSize = 8;  // Adjust pattern size to 8x8 for better tessellation
    int center = patternSize / 2; // To align the flower center

    // Loop through the bitmap's pixel space
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Adjust the pattern logic to fit the tessellation grid
            int dx = x % patternSize;
            int dy = y % patternSize;

            // Define the pattern with a more regular distribution and alignment
            if ((dx == center && dy == center) || // center of flower
                (dx == center - 1 && dy == center) || // left of center
                (dx == center + 1 && dy == center) || // right of center
                (dx == center && dy == center - 1) || // above center
                (dx == center && dy == center + 1) || // below center
                (dx == center - 1 && dy == center - 1) || // top-left
                (dx == center + 1 && dy == center + 1) || // bottom-right
                (dx == center - 1 && dy == center + 1) || // bottom-left
                (dx == center + 1 && dy == center - 1)) { // top-right
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }

    // Return the generated bitmap
    return bitmap;
}



Bitmap* CreateDensePlaid(int width, int height, const Color& stripeColor1, const Color& stripeColor2, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int stripeWidth = GRID_SIZE / 8; // Thinner stripes for a denser look

    SolidBrush brush1(stripeColor1);
    SolidBrush brush2(stripeColor2);

    for (int x = 0; x < width; x += stripeWidth * 2) {
        graphics.FillRectangle(&brush1, x, 0, stripeWidth, height);
    }

    for (int y = 0; y < height; y += stripeWidth * 2) {
        graphics.FillRectangle(&brush2, 0, y, width, stripeWidth);
    }

    return bitmap;
}

Bitmap* CreateDiagonalStripePattern(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int stripeWidth = GRID_SIZE / 3;

    for (int x = -height; x < width; x += stripeWidth * 2) {
        SolidBrush brush1(color1);
        SolidBrush brush2(color2);

        // First stripe
        graphics.FillPolygon(&brush1, new Point[4]{
            {x, 0},
            {x + stripeWidth, 0},
            {x + stripeWidth - height, height},
            {x - height, height}
            }, 4);

        // Second stripe
        graphics.FillPolygon(&brush2, new Point[4]{
            {x + stripeWidth, 0},
            {x + 2 * stripeWidth, 0},
            {x + 2 * stripeWidth - height, height},
            {x + stripeWidth - height, height}
            }, 4);
    }

    return bitmap;
}



Bitmap* CreateStarfield(int width, int height, const Color& starColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    SolidBrush starBrush(starColor);

    // We will adjust the number of stars to create a balanced density
    int numStars = (width * height) / 200;

    // Randomly distribute stars across the grid
    for (int i = 0; i < numStars; ++i) {
        // Random position with the caveat that they wrap across the borders
        int x = rand() % (width / 2) + (i % 2 == 0 ? 0 : width / 2); // Alternates between left and right halves
        int y = rand() % (height / 2) + (i % 2 == 0 ? 0 : height / 2); // Alternates between top and bottom halves

        int size = (rand() % 3) + 1; // Small variation in size

        // Ensure stars are distributed in a way that tessellates without abrupt borders
        if (x + size > width) x = width - size;
        if (y + size > height) y = height - size;

        graphics.FillEllipse(&starBrush, x, y, size, size);
    }

    return bitmap;
}

Bitmap* CreateDottedHalftone(int width, int height, const Color& dotColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    SolidBrush dotBrush(dotColor);

    // Define the grid size based on the desired resolution
    const int gridSize = 8; // Using a grid of 8x8 for more fine-tuned tessellation within 32x32

    for (int y = 0; y < height; y += gridSize) {
        for (int x = 0; x < width; x += gridSize) {
            // Vary the radius based on the position for shading effect
            int radius = (x + y) % (gridSize * 2) / 2 + 1; // Dynamic radius with shading

            // Make sure the dot fits within bounds of the tile
            if (x + radius < width && y + radius < height) {
                graphics.FillEllipse(&dotBrush, x, y, radius, radius);
            }
        }
    }

    return bitmap;
}









Bitmap* CreateGearsPattern(int width, int height, const Color& gearColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    // Adjust the grid size to fit better in a 32x32 space, ensuring proper tessellation
    int gearSize = width / 4;  // Adjusted gear size, assuming 32x32 pixel workspace
    SolidBrush gearBrush(gearColor);

    // Loop over the area, ensuring gears are positioned properly for tessellation
    for (int y = 0; y < height; y += gearSize * 2) {
        for (int x = 0; x < width; x += gearSize * 2) {
            graphics.FillEllipse(&gearBrush, x, y, gearSize, gearSize);            // Top-left gear
            graphics.FillEllipse(&gearBrush, x + gearSize, y + gearSize, gearSize, gearSize);  // Bottom-right gear
        }
    }

    return bitmap;
}

Bitmap* CreateLinkedRings(int width, int height, const Color& ringColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    // Set the ring size proportionally to the 32x32 grid size.
    int ringRadius = width / 6; // Adjusted to fit the 32x32 workspace.
    int spacing = ringRadius * 3; // Spacing between centers of rings, ensuring tessellation.

    Pen ringPen(ringColor, 2);

    // Adjust the outer bounds of the pattern.
    for (int y = ringRadius; y < height - ringRadius; y += spacing) {
        for (int x = ringRadius; x < width - ringRadius; x += spacing) {
            // Draw two linked rings at each position.
            graphics.DrawEllipse(&ringPen, x - ringRadius, y - ringRadius, ringRadius * 2, ringRadius * 2);  // Outer ring
            graphics.DrawEllipse(&ringPen, x - ringRadius / 2, y - ringRadius / 2, ringRadius, ringRadius); // Inner ring
        }
    }

    return bitmap;
}






Bitmap* CreateFractalHalftone(int width, int height, const Color& dotColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    SolidBrush dotBrush(dotColor);

    const int gridSize = 8;

    // Recursive function to simulate fractal behavior
    auto drawFractal = [&](int x, int y, int size, auto&& drawFractal) -> void {
        if (size <= 1) return; // Base case for recursion
        graphics.FillEllipse(&dotBrush, x, y, size, size);
        drawFractal(x + size / 2, y + size / 2, size / 2, drawFractal); // Draw smaller fractal
        drawFractal(x - size / 2, y - size / 2, size / 2, drawFractal); // Draw another smaller fractal
        };

    for (int y = 0; y < height; y += gridSize) {
        for (int x = 0; x < width; x += gridSize) {
            int size = (x + y) % 4 + 1; // Vary the dot size
            drawFractal(x, y, size * gridSize, drawFractal); // Call the recursive fractal drawing
        }
    }

    return bitmap;
}

Bitmap* CreateRadialZigzagHalftone(int width, int height, const Color& lineColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    Pen linePen(lineColor, 1);

    const int gridSize = 8;

    for (int y = 0; y < height; y += gridSize) {
        for (int x = 0; x < width; x += gridSize) {
            // Radial zigzag effect: change direction based on position
            int offsetX = (x + y) % (gridSize * 2) - gridSize / 2;
            int offsetY = (y + x) % (gridSize * 2) - gridSize / 2;

            // Draw zigzag from the center outwards
            for (int i = 0; i < 5; ++i) {
                graphics.DrawLine(&linePen, x + offsetX * i, y + offsetY * i, x + offsetX * (i + 1), y + offsetY * (i + 1));
            }
        }
    }

    return bitmap;
}



Bitmap* CreateLinkedSquares(int width, int height, const Color& squareColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int squareSize = width / 6; // Adjusted for a 32x32 grid
    int spacing = squareSize * 3; // Ensures a structured layout

    Pen squarePen(squareColor, 2);

    for (int y = squareSize; y < height - squareSize; y += spacing) {
        for (int x = squareSize; x < width - squareSize; x += spacing) {
            // Draw interlocking squares
            graphics.DrawRectangle(&squarePen, x - squareSize, y - squareSize, squareSize * 2, squareSize * 2); // Outer square
            graphics.DrawRectangle(&squarePen, x - squareSize / 2, y - squareSize / 2, squareSize, squareSize); // Inner square
        }
    }

    return bitmap;
}

Bitmap* CreateGeodesicHoneycomb(int width, int height, const Color& stripeColor1, const Color& stripeColor2, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int hexWidth = GRID_SIZE / 6;
    int hexHeight = static_cast<int>(hexWidth * sqrt(3) / 2);
    SolidBrush brush1(stripeColor1);
    SolidBrush brush2(stripeColor2);

    for (int y = 0; y < height; y += hexHeight) {
        for (int x = 0; x < width; x += hexWidth * 3) {
            int offsetY = (x / (hexWidth * 3)) % 2 == 0 ? 0 : hexHeight / 2;
            Point points[6] = {
                Point(x, y + offsetY),
                Point(x + hexWidth / 2, y + offsetY + hexHeight / 2),
                Point(x + hexWidth * 2, y + offsetY + hexHeight / 2),
                Point(x + hexWidth * 3, y + offsetY),
                Point(x + hexWidth * 2, y + offsetY - hexHeight / 2),
                Point(x + hexWidth / 2, y + offsetY - hexHeight / 2)
            };
            graphics.FillPolygon(((x + y) / (hexWidth * 3)) % 2 == 0 ? &brush1 : &brush2, points, 6);
        }
    }

    return bitmap;
}

Bitmap* CreateFlowerVariationDiamond(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);

    // Adjust the pattern size to fit within the 32x32 grid for tessellation
    int patternSize = 8;  // Adjust pattern size to 8x8 for better tessellation
    int center = patternSize / 2; // To align the diamond center

    // Loop through the bitmap's pixel space
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Adjust the pattern logic to fit the tessellation grid
            int dx = x % patternSize;
            int dy = y % patternSize;

            // Define a diamond shape within the grid
            if ((dx == center && dy == center) || // center
                (dx == center - 1 && dy == center) || (dx == center + 1 && dy == center) || // left and right
                (dx == center && dy == center - 1) || (dx == center && dy == center + 1) || // top and bottom
                (dx == center - 1 && dy == center - 1) || (dx == center + 1 && dy == center - 1) || // upper diagonals
                (dx == center - 1 && dy == center + 1) || (dx == center + 1 && dy == center + 1) || // lower diagonals
                (dx == center - 2 && dy == center) || (dx == center + 2 && dy == center) || // extended left-right
                (dx == center && dy == center - 2) || (dx == center && dy == center + 2)) { // extended top-bottom
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }

    // Return the generated bitmap
    return bitmap;
}

Bitmap* CreateFlowerVariationWave(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);

    // Adjust the pattern size to fit within the 32x32 grid for tessellation
    int patternSize = 8;
    int center = patternSize / 2;

    // Loop through the bitmap's pixel space
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x % patternSize;
            int dy = y % patternSize;

            // Create a sinusoidal wave effect for the pattern
            if ((dx + dy) % patternSize == center || (dx + dy) % patternSize == (center + 1)) {
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }

    return bitmap;
}
Bitmap* CreateFlowerVariationConcentric(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);

    // Adjust the pattern size to fit within the 32x32 grid for tessellation
    int patternSize = 8;
    int centerX = patternSize / 2;
    int centerY = patternSize / 2;

    // Loop through the bitmap's pixel space
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x % patternSize;
            int dy = y % patternSize;

            // Define concentric circle pattern
            if (abs(dx - centerX) == abs(dy - centerY)) {
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }

    return bitmap;
}
Bitmap* CreateFlowerVariation14(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (((x / 4 + y / 4) % 2 == 0) && ((x * y) % 9 != 0)) { // Shattered glass, irregular grid with "cracks"
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }
    return bitmap;
}
Bitmap* CreateFlowerVariation12(int width, int height, const Color& flowerColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    SolidBrush backgroundBrush(backgroundColor);
    graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

    SolidBrush flowerBrush(flowerColor);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (((x / 6) % 2 == (y / 4) % 2)) { // Snakeskin-like pattern, alternating hexagonal-like shapes
                graphics.FillRectangle(&flowerBrush, x, y, 1, 1);
            }
        }
    }
    return bitmap;
}

Bitmap* CreateSmoothZigzagPattern(int width, int height, const Color& lineColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    Pen pen(lineColor, 2);
    int zigzagHeight = GRID_SIZE / 6;

    // Create a smooth zigzag pattern across the bitmap
    for (int y = 0; y < height; y += zigzagHeight) {
        for (int x = 0; x < width; x++) {
            int zigzagOffset = (x / GRID_SIZE) % 2 == 0 ? zigzagHeight : -zigzagHeight;
            PointF points[2] = {
                PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y + zigzagOffset)),
                PointF(static_cast<Gdiplus::REAL>(x + 1), static_cast<Gdiplus::REAL>(y + zigzagOffset))
            };
            graphics.DrawLines(&pen, points, 2);
        }
    }

    return bitmap;
}


//NEW AND UNTESTED

Bitmap* CreateSolidColorBlock(int width, int height, const Color& color) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    // Fill the entire bitmap with the given color
    SolidBrush brush(color);
    graphics.FillRectangle(&brush, 0, 0, width, height);

    return bitmap;
}




Bitmap* CreateTextBitmap(char character, const Color& color) {
    int width = 32, height = 32;
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    // Fill background with transparency or a solid color if needed
    graphics.Clear(Color(0, 0, 0, 0)); // Transparent background

    // Convert char to a wide character string
    std::wstring wstr(1, character);

    FontFamily fontFamily(L"Consolas");
    Font font(L"Consolas", 32, FontStyleBold, UnitPixel);

    // Draw the character
    SolidBrush brush(color);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    // Ensure RectF uses floating-point values
    RectF rect(0.0f, 0.0f, static_cast<Gdiplus::REAL>(width), static_cast<Gdiplus::REAL>(height));
    graphics.DrawString(wstr.c_str(), -1, &font, rect, &format, &brush);

    return bitmap;
}


///////////////


Bitmap* CreateOverlappingGearsAndChevrons(int width, int height, const Color& gearColor, const Color& chevronColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int gridSize = width / 4;
    SolidBrush gearBrush(gearColor);
    SolidBrush chevronBrush(chevronColor);

    for (int y = 0; y < height; y += gridSize) {
        for (int x = 0; x < width; x += gridSize) {
            // Draw alternating gears and chevrons
            if ((x / gridSize + y / gridSize) % 2 == 0) {
                graphics.FillEllipse(&gearBrush, x, y, gridSize, gridSize);
            }
            else {
                Point chevron[3] = {
                    Point(x, y + gridSize),
                    Point(x + gridSize / 2, y),
                    Point(x + gridSize, y + gridSize)
                };
                graphics.FillPolygon(&chevronBrush, chevron, 3);
            }
        }
    }

    return bitmap;
}
Bitmap* CreateGearsWithChevronSpokes(int width, int height, const Color& gearColor, const Color& spokeColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int gearSize = width / 4;
    SolidBrush gearBrush(gearColor);
    SolidBrush spokeBrush(spokeColor);

    for (int y = 0; y < height; y += gearSize * 2) {
        for (int x = 0; x < width; x += gearSize * 2) {
            // Draw solid gear
            graphics.FillEllipse(&gearBrush, x, y, gearSize, gearSize);

            // Draw chevron-like spokes
            Point spokes[3] = {
                Point(x + gearSize / 2, y),         // Top center
                Point(x + (3 * gearSize) / 4, y + gearSize / 2),
                Point(x + gearSize / 4, y + gearSize / 2)
            };
            graphics.FillPolygon(&spokeBrush, spokes, 3);
        }
    }

    return bitmap;
}
Bitmap* CreateGearsWithChevronCutouts(int width, int height, const Color& gearColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    int gearSize = width / 4;  // Gear size assuming a 32x32 pattern
    SolidBrush gearBrush(gearColor);
    SolidBrush backgroundBrush(backgroundColor);

    for (int y = 0; y < height; y += gearSize * 2) {
        for (int x = 0; x < width; x += gearSize * 2) {
            // Draw solid gear
            graphics.FillEllipse(&gearBrush, x, y, gearSize, gearSize);

            // Chevron cutout
            Point chevron[3] = {
                Point(x + gearSize / 4, y + gearSize / 2),
                Point(x + gearSize / 2, y + gearSize / 4),
                Point(x + (3 * gearSize) / 4, y + gearSize / 2)
            };
            graphics.FillPolygon(&backgroundBrush, chevron, 3);
        }
    }

    return bitmap;
}
Bitmap* CreateHerringboneChevronMix(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int chevronWidth = width / 5;
    int chevronHeight = height / 5;

    for (int y = 0; y < height; y += chevronHeight) {
        for (int x = (y / chevronHeight) % 2 == 0 ? 0 : -chevronWidth / 2; x < width + chevronWidth; x += chevronWidth) {
            SolidBrush brush((x / chevronWidth + y / chevronHeight) % 2 == 0 ? color1 : color2);

            Point points[3] = {
                Point(x, y),
                Point(x + chevronWidth, y + chevronHeight / 2),
                Point(x, y + chevronHeight)
            };

            graphics.FillPolygon(&brush, points, 3);

            Point inversePoints[3] = {
                Point(x + chevronWidth, y),
                Point(x, y + chevronHeight / 2),
                Point(x + chevronWidth, y + chevronHeight)
            };

            graphics.FillPolygon(&brush, inversePoints, 3);
        }
    }

    return bitmap;
}
Bitmap* CreateDiagonalChevronWeave(int width, int height, const Color& color1, const Color& color2) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);

    int chevronWidth = width / 6;
    int chevronHeight = height / 6;

    for (int y = 0; y < height; y += chevronHeight) {
        for (int x = (y / chevronHeight) % 2 == 0 ? 0 : chevronWidth / 2; x < width; x += chevronWidth) {
            SolidBrush brush((x / chevronWidth + y / chevronHeight) % 2 == 0 ? color1 : color2);

            // Create left diagonal
            Point leftPoints[3] = {
                Point(x, y),
                Point(x + chevronWidth / 2, y + chevronHeight),
                Point(x, y + chevronHeight)
            };
            graphics.FillPolygon(&brush, leftPoints, 3);

            // Create right diagonal
            Point rightPoints[3] = {
                Point(x + chevronWidth, y),
                Point(x + chevronWidth / 2, y + chevronHeight),
                Point(x + chevronWidth, y + chevronHeight)
            };
            graphics.FillPolygon(&brush, rightPoints, 3);
        }
    }

    return bitmap;
}




Bitmap* CreateHybridOrganicPattern(int width, int height, const Color& lineColor, const Color& backgroundColor) {
    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.Clear(backgroundColor);

    Pen pen(lineColor, 2);

    int hexWidth = width / 6;
    int hexHeight = static_cast<int>(hexWidth * sqrt(3) / 2);

    float distortionScale = hexWidth * 0.2f; // Scale factor for distortions

    // Inline noise function for organic distortions
    auto noise = [](int x, int y, float scale) -> float {
        return std::sin(x * 0.1f) * std::cos(y * 0.1f) * scale;
        };

    // Draw distorted honeycomb pattern
    for (int y = 0; y < height; y += hexHeight) {
        for (int x = 0; x < width; x += hexWidth * 3) {
            int offsetY = (x / (hexWidth * 3)) % 2 == 0 ? 0 : hexHeight / 2;

            // Apply unique distortion to each vertex (explicitly casting to int)
            Point points[6] = {
                Point(x + static_cast<int>(noise(x, y, distortionScale)), y + offsetY + static_cast<int>(noise(y, x, distortionScale))),
                Point(x + hexWidth / 2 + static_cast<int>(noise(x + 1, y, distortionScale)), y + offsetY + hexHeight / 2 + static_cast<int>(noise(y + 1, x, distortionScale))),
                Point(x + hexWidth * 2 + static_cast<int>(noise(x + 2, y, distortionScale)), y + offsetY + hexHeight / 2 + static_cast<int>(noise(y + 2, x, distortionScale))),
                Point(x + hexWidth * 3 + static_cast<int>(noise(x + 3, y, distortionScale)), y + offsetY + static_cast<int>(noise(y + 3, x, distortionScale))),
                Point(x + hexWidth * 2 + static_cast<int>(noise(x + 4, y, distortionScale)), y + offsetY - hexHeight / 2 + static_cast<int>(noise(y + 4, x, distortionScale))),
                Point(x + hexWidth / 2 + static_cast<int>(noise(x + 5, y, distortionScale)), y + offsetY - hexHeight / 2 + static_cast<int>(noise(y + 5, x, distortionScale)))
            };

            graphics.DrawPolygon(&pen, points, 6);
        }
    }

    return bitmap;
}









void LoadSprites() {

    // **Whites & Light Neutrals**  
    Color white_color(255, 255, 255);            // Pure White  
    Color ivory_color(255, 255, 240);            // Soft Ivory  
    Color beige_color(245, 245, 220);            // Neutral Beige  
    Color wheat_color(245, 222, 179);            // Warm Wheat  
    Color tan_color(210, 180, 140);              // Sandy Brown  

    // **Yellows & Oranges**  
    Color lemon_color(255, 247, 0);              // Bright Lemon Yellow  
    Color amber_color(255, 191, 0);              // Vivid Amber  
    Color gold_color(255, 215, 0);               // Classic Gold  
    Color saffron_color(244, 196, 48);           // Deep Saffron Yellow  
    Color orange_color(255, 165, 0);             // Pure Orange  
    Color burntOrange_color(204, 85, 0);         // Deep Burnt Orange  
    Color rust_color(183, 65, 14);               // Rusty Orange  

    // **Reds & Pinks**  
    Color lightSalmon_color(255, 160, 122);      // Soft Light Salmon  
    Color vermilion_color(227, 66, 52);          // Bright Vermilion  
    Color coral_color(255, 50, 71);              // Warm Coral  
    Color crimson_color(220, 20, 60);            // Deep Crimson  
    Color red_color(255, 0, 0);                  // True Red  
    Color maroon_color(128, 0, 0);               // Dark Maroon  
    Color hotPink_color(255, 105, 180);          // Vibrant Hot Pink  
    Color magenta_color(255, 0, 255);            // Electric Magenta  

    // **Earth Tones & Browns**  
    Color saddleBrown_color(139, 69, 19);        // Deep Saddle Brown  
    Color sepia_color(112, 66, 20);              // Warm Sepia  
    Color sienna_color(160, 82, 45);             // Rich Sienna  
    Color reddishBrown_color(210, 100, 10);      // Rusty Reddish Brown  
    Color brown_color(165, 42, 42);              // Standard Brown  
    Color mahogany_color(192, 64, 0);            // Deep Mahogany  
    Color chestnut_color(205, 92, 92);           // Muted Chestnut  

    // **Greens - Grass & Foliage**  
    Color chartreuse_color(127, 255, 0);         // Bright Chartreuse  
    Color brightGreen_color(76, 175, 80);        // Lush Bright Green  
    Color springGreen_color(0, 255, 127);        // Vivid Spring Green  
    Color lightGreen_color(144, 238, 144);       // Soft Light Green  
    Color yellowGreen_color(154, 205, 50);       // Yellow-Green  
    Color limeGreen_color(50, 205, 50);          // Vivid Lime Green  
    Color olive_color(107, 142, 35);             // True Olive Green  
    Color mossGreen_color(173, 223, 173);        // Soft Moss Green  
    Color darkOliveGreen_color(85, 107, 47);     // Deep Dark Olive Green  
    Color forestGreen_color(34, 139, 34);        // Rich Forest Green  
    Color deepGreen_color(0, 100, 0);            // Dark Forest Green  
    Color seaGreen_color(46, 139, 87);           // Balanced Sea Green  

    // **Blues - Water & Sky**  
    Color skyBlue_color(135, 206, 235);          // Clear Sky Blue  
    Color freshWaterBlue_color(173, 216, 230);   // Fresh Water Blue  
    Color deepFreshWaterBlue_color(135, 206, 250); // Deep Fresh Water Blue  
    Color cornflowerBlue_color(100, 149, 237);   // Soft Cornflower Blue  
    Color steelBlue_color(70, 130, 180);         // Sea Water Blue  
    Color dodgerBlue_color(30, 144, 255);        // Bright Dodger Blue  
    Color royalBlue_color(65, 105, 225);         // Deep Royal Blue  
    Color pureBlue_color(0, 0, 255);             // True Blue  
    Color midnightBlue_color(25, 25, 112);       // Dark Midnight Blue  
    Color deepSeaBlue_color(0, 0, 139);          // Deep Sea Blue  

    // **Purples & Violets**  
    Color lavender_color(230, 230, 250);         // Soft Lavender  
    Color thistle_color(216, 191, 216);          // Muted Thistle  
    Color orchid_color(218, 112, 214);           // Rich Orchid  
    Color violet_color(238, 130, 238);           // Pure Violet  
    Color darkOrchid_color(153, 50, 204);        // Deep Dark Orchid  
    Color indigo_color(75, 0, 130);              // Intense Indigo  

    // **Neutrals - Rocks, Shadows, & Structures**  
    Color lightGray_color(211, 211, 211);        // Soft Light Gray  
    Color gray_color(128, 128, 128);             // Neutral Gray  
    Color lightSlateGray_color(160, 150, 150);   // Muted Light Slate Gray  
    Color ashGray_color(178, 190, 181);          // Soft Ash Gray  
    Color slateGray_color(112, 128, 144);        // Balanced Slate Gray  
    Color darkGray_color(169, 169, 169);         // Dark Gray  
    Color dimGray_color(105, 105, 105);          // Standard Dim Gray  
    Color charcoal_color(50, 50, 50);            // Charcoal Gray  
    Color gunmetalGray_color(42, 52, 57);        // Cool Gunmetal Gray  
    Color veryDarkGray_color(30, 30, 30);        // Very Dark Gray  
    Color jetBlack_color(20, 20, 20);            // Deep Jet Black  


    //pattern creation via method and color

    checkerboard_pattern_1 = CreateCheckerboardPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    chevron_pattern_1 = CreateChevronPattern(GRID_SIZE, GRID_SIZE, gray_color, white_color);
    concentricdiamond_pattern_1 = CreateConcentricDiamond(GRID_SIZE, GRID_SIZE, 1, white_color, gray_color);
    denseplaid_pattern_1 = CreateDensePlaid(GRID_SIZE, GRID_SIZE, jetBlack_color, gray_color, white_color);
    diagonalgrid_pattern_1 = GenerateDiagonalGrid(GRID_SIZE, GRID_SIZE, white_color, jetBlack_color);
    diagonalherringbone_pattern_1 = CreateDiagonalHerringbone(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    diagonalzigzag_pattern_1 = CreateDiagonalZigzagPattern(GRID_SIZE, GRID_SIZE, white_color, jetBlack_color);
    dottedhalftone_pattern_1 = CreateDottedHalftone(GRID_SIZE, GRID_SIZE, white_color, gray_color);
    diamond_pattern_1 = CreateDiamondPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    flowervar1_pattern_1 = CreateFlowerVariation1(GRID_SIZE, GRID_SIZE, jetBlack_color, white_color);
    flowervar2_pattern_1 = CreateFlowerVariation2(GRID_SIZE, GRID_SIZE, jetBlack_color, white_color);
    flowervar3_pattern_1 = CreateFlowerVariation3(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    flower_pattern_1 = CreateFlowerPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    gears_pattern_1 = CreateGearsPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    herringbone_pattern_1 = CreateHerringbonePattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    offsetdiagonallines_pattern_1 = CreateOffsetDiagonalLines(GRID_SIZE, GRID_SIZE, 10, gray_color, jetBlack_color);
    offsetherringbone_pattern_1 = CreateOffsetHerringbone(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    plaid_pattern_1 = CreatePlaidPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color, white_color);
    smoothzigzag_pattern_1 = CreateSmoothZigzagPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    vstripe_pattern_1 = CreateVStripePattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    zigzagstripes_pattern_1 = CreateZigzagStripes(GRID_SIZE, GRID_SIZE, 1, 1, white_color, gray_color);
    zigzagherringbone_pattern_1 = CreateZigZagHerringbone(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    linkedrings_pattern_1 = CreateLinkedRings(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    flowervar12_pattern_1 = CreateFlowerVariation12(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    flowervar14_pattern_1 = CreateFlowerVariation14(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    concentricflowers_pattern_1 = CreateFlowerVariationConcentric(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    flowerwave_pattern_1 = CreateFlowerVariationWave(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    flowerdiamond_pattern_1 = CreateFlowerVariationDiamond(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    honeycomb_pattern_1 = CreateGeodesicHoneycomb(GRID_SIZE, GRID_SIZE, white_color, gray_color, jetBlack_color);
    linkedsquares_pattern_1 = CreateLinkedSquares(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    radialzigzag_pattern_1 = CreateRadialZigzagHalftone(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    fractalhalftone_pattern_1 = CreateFractalHalftone(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);

    red_solid_pattern = CreateSolidColorBlock(GRID_SIZE, GRID_SIZE, red_color);
    x_text_pattern = CreateTextBitmap('X', red_color);

    hybridorganic_pattern = CreateHybridOrganicPattern(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    diagonalchevron_pattern = CreateDiagonalChevronWeave(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    herringbonechevron_pattern = CreateHerringboneChevronMix(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    gearswithcuts_pattern = CreateGearsWithChevronCutouts(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color);
    gearswithspokes_pattern = CreateGearsWithChevronSpokes(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color, red_color);
    gearsandchevrons_pattern = CreateOverlappingGearsAndChevrons(GRID_SIZE, GRID_SIZE, gray_color, jetBlack_color, red_color);

}

const int CHUNK_ROWS = 4; // Number of chunks in rows
const int CHUNK_COLS = 4; // Number of chunks in columns
const int CHUNK_WIDTH = WIDTH / CHUNK_COLS;  // Width of each chunk
const int CHUNK_HEIGHT = HEIGHT / CHUNK_ROWS; // Height of each chunk

// Define the block rendering map globally or statically
std::unordered_map<int, std::function<void(Graphics&, const Block&)>> blockRenderers;

///create block type for every pattern example with color configuration that can show adequet color contrast

void InitializeBlockRenderers() {
    blockRenderers[1] = [](Graphics& g, const Block& b) { g.DrawImage(chevron_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[2] = [](Graphics& g, const Block& b) { g.DrawImage(checkerboard_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[3] = [](Graphics& g, const Block& b) { g.DrawImage(flowervar2_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[4] = [](Graphics& g, const Block& b) { g.DrawImage(flowervar3_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[5] = [](Graphics& g, const Block& b) { g.DrawImage(flower_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[6] = [](Graphics& g, const Block& b) { g.DrawImage(flowervar1_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[7] = [](Graphics& g, const Block& b) { g.DrawImage(diamond_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[8] = [](Graphics& g, const Block& b) { g.DrawImage(offsetherringbone_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[9] = [](Graphics& g, const Block& b) { g.DrawImage(smoothzigzag_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[10] = [](Graphics& g, const Block& b) { g.DrawImage(zigzagherringbone_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[11] = [](Graphics& g, const Block& b) { g.DrawImage(vstripe_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[12] = [](Graphics& g, const Block& b) { g.DrawImage(plaid_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[13] = [](Graphics& g, const Block& b) { g.DrawImage(dottedhalftone_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[14] = [](Graphics& g, const Block& b) { g.DrawImage(offsetdiagonallines_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[15] = [](Graphics& g, const Block& b) { g.DrawImage(diagonalgrid_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[16] = [](Graphics& g, const Block& b) { g.DrawImage(herringbone_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[17] = [](Graphics& g, const Block& b) { g.DrawImage(denseplaid_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[18] = [](Graphics& g, const Block& b) { g.DrawImage(zigzagstripes_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[19] = [](Graphics& g, const Block& b) { g.DrawImage(diagonalzigzag_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[20] = [](Graphics& g, const Block& b) { g.DrawImage(gears_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[21] = [](Graphics& g, const Block& b) { g.DrawImage(concentricdiamond_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[22] = [](Graphics& g, const Block& b) { g.DrawImage(diagonalherringbone_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[23] = [](Graphics& g, const Block& b) { g.DrawImage(linkedrings_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[24] = [](Graphics& g, const Block& b) { g.DrawImage(flowervar12_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[25] = [](Graphics& g, const Block& b) { g.DrawImage(flowervar14_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[26] = [](Graphics& g, const Block& b) { g.DrawImage(concentricflowers_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[27] = [](Graphics& g, const Block& b) { g.DrawImage(flowerwave_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[28] = [](Graphics& g, const Block& b) { g.DrawImage(flowerdiamond_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[29] = [](Graphics& g, const Block& b) { g.DrawImage(honeycomb_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[30] = [](Graphics& g, const Block& b) { g.DrawImage(linkedsquares_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[31] = [](Graphics& g, const Block& b) { g.DrawImage(radialzigzag_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[32] = [](Graphics& g, const Block& b) { g.DrawImage(fractalhalftone_pattern_1, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[33] = [](Graphics& g, const Block& b) { g.DrawImage(red_solid_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[34] = [](Graphics& g, const Block& b) { g.DrawImage(x_text_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[35] = [](Graphics& g, const Block& b) { g.DrawImage(hybridorganic_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[36] = [](Graphics& g, const Block& b) { g.DrawImage(gearswithcuts_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[37] = [](Graphics& g, const Block& b) { g.DrawImage(gearswithspokes_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[38] = [](Graphics& g, const Block& b) { g.DrawImage(gearsandchevrons_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[39] = [](Graphics& g, const Block& b) { g.DrawImage(diagonalchevron_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };
    blockRenderers[40] = [](Graphics& g, const Block& b) { g.DrawImage(herringbonechevron_pattern, b.x, b.y, GRID_SIZE, GRID_SIZE); };

}


void DrawScene(HDC hdc) {
    // Initialize hdcMem if it hasn't been done yet
    if (hdcMem == NULL) {
        hdcMem = CreateCompatibleDC(hdc);
        hbmMem = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
        hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    }

    // Fill background
    RECT backgroundRect = { 0, 0, WIDTH, HEIGHT };
    FillRect(hdcMem, &backgroundRect, backgroundBrush);

    // Create a GDI+ Graphics object from the memory device context
    Graphics graphics(hdcMem);

    // Initialize block renderers if not already done
    static bool initialized = false;
    if (!initialized) {
        InitializeBlockRenderers();
        initialized = true;
    }

    // Loop through each chunk and each block in the chunk
    for (int chunkRow = 0; chunkRow < CHUNK_ROWS; ++chunkRow) {
        for (int chunkCol = 0; chunkCol < CHUNK_COLS; ++chunkCol) {
            int chunkXStart = chunkCol * CHUNK_WIDTH;
            int chunkYStart = chunkRow * CHUNK_HEIGHT;
            int chunkXEnd = chunkXStart + CHUNK_WIDTH;
            int chunkYEnd = chunkYStart + CHUNK_HEIGHT;

            for (const Block& block : blocks) {
                if (block.type != -1) { // Ignore empty blocks
                    if (block.x >= chunkXStart && block.x < chunkXEnd &&
                        block.y >= chunkYStart && block.y < chunkYEnd) {

                        RECT blockRect = { block.x, block.y, block.x + GRID_SIZE, block.y + GRID_SIZE };

                        // Check if block type has a renderer
                        auto it = blockRenderers.find(block.type);
                        if (it != blockRenderers.end()) {
                            it->second(graphics, block); // Call the corresponding rendering function
                        }
                    }
                }
            }
        }
    }


    // Copy the memory device context to the actual device context
    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hdcMem, 0, 0, SRCCOPY);
}


std::wstring GetSaveFilePath() {
    wchar_t path[MAX_PATH]; // Buffer for path
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, path))) { // Get the "Documents" folder
        std::wstring savePath = std::wstring(path) + L"\\lgrsave.txt"; // Append file name
        return savePath; // Return the full save path
    }
    return L"lgrsave.txt"; // Fallback path if SHGetFolderPath fails
}
void CleanupSprites() {


    //rename to new pattern ids 

    delete checkerboard_pattern_1;
    delete chevron_pattern_1;
    delete concentricdiamond_pattern_1;
    delete denseplaid_pattern_1;
    delete diagonalgrid_pattern_1;
    delete diagonalherringbone_pattern_1;
    delete diagonalzigzag_pattern_1;
    delete dottedhalftone_pattern_1;
    delete diamond_pattern_1;
    delete flowervar1_pattern_1;
    delete flowervar2_pattern_1;
    delete flowervar3_pattern_1;
    delete flower_pattern_1;
    delete gears_pattern_1;
    delete herringbone_pattern_1;
    delete offsetdiagonallines_pattern_1;
    delete offsetherringbone_pattern_1;
    delete plaid_pattern_1;
    delete smoothzigzag_pattern_1;
    delete vstripe_pattern_1;
    delete zigzagherringbone_pattern_1;
    delete zigzagstripes_pattern_1;
    delete linkedrings_pattern_1;
    delete flowervar12_pattern_1;
    delete flowervar14_pattern_1;
    delete concentricflowers_pattern_1;
    delete flowerwave_pattern_1;
    delete flowerdiamond_pattern_1;
    delete honeycomb_pattern_1;
    delete linkedsquares_pattern_1;
    delete radialzigzag_pattern_1;
    delete fractalhalftone_pattern_1;
    delete red_solid_pattern;
    delete x_text_pattern;
    delete hybridorganic_pattern;
    delete gearswithcuts_pattern;
    delete gearswithspokes_pattern;
    delete gearsandchevrons_pattern;
    delete diagonalchevron_pattern;
    delete herringbonechevron_pattern;









    if (hbmMem) {
        DeleteObject(hbmMem); // Delete the compatible bitmap
    }
    if (hdcMem) {
        SelectObject(hdcMem, hbmOld); // Restore the old bitmap
        DeleteDC(hdcMem); // Delete the memory device context
    }


    // Optionally reset global pointers to avoid dangling references
    hdcMem = nullptr;
    hbmMem = nullptr;
    hbmOld = nullptr;
    // Reset any GDI+ bitmaps if you have them
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow) {//
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    LoadSprites();
    WNDCLASS wc = {}; // Window class structure
    wc.lpfnWndProc = WindowProc; // Set the window procedure
    wc.hInstance = hInstance;    // Handle to the instance
    wc.lpszClassName = CLASS_NAME; // Name of the window class
    // Set background color -- possibly redundant with other background color that is drawn in
    RegisterClass(&wc); // Register the window class
    RECT rect = { 0, 0, WIDTH, HEIGHT };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(
        0,                           // Optional styles
        CLASS_NAME,                   // Window class name
        L"LGR",                 // Window title
        WS_OVERLAPPEDWINDOW,          // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, // Position and size
        rect.right - rect.left,       // Adjusted width
        rect.bottom - rect.top,       // Adjusted height
        NULL, NULL, hInstance, NULL
    );
    if (hwnd == NULL) { // If window creation failed
        return 0;
    }

    InitializeBrushes();

    // Initialize memory DC once
    InitializeMemoryDC(GetDC(NULL));
    ShowWindow(hwnd, nCmdShow); // Show the window
    UpdateWindow(hwnd);         // Update the window
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { // Message loop
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    return 0; // Exit the application
}