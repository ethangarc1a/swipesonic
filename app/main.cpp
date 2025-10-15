#include "raylib.h"
#include <string>

// Simple “card” placeholder so we can confirm window + drawing works.
int main() {
    const int screenWidth = 900;
    const int screenHeight = 600;

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Swipe Sonic — Hello Window");
    SetTargetFPS(60);

    // Card geometry
    const int cardW = 520;
    const int cardH = 360;
    Rectangle cardRect = {
        (float)(screenWidth - cardW) / 2.0f,
        (float)(screenHeight - cardH) / 2.0f,
        (float)cardW,
        (float)cardH
    };

    // Placeholder metadata (we’ll wire real data later)
    std::string title  = "Track Title";
    std::string artist = "Artist Name";
    std::string genre  = "Genre • 110 BPM";

    while (!WindowShouldClose()) {
        // Input (wired later)
        bool like  = (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D));
        bool skip  = (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A));
        bool playPause = IsKeyPressed(KEY_SPACE);
        (void)like; (void)skip; (void)playPause;

        BeginDrawing();
        ClearBackground(Color{ 18, 18, 20, 255 });

        // Title
        DrawText("Swipe Sonic", 24, 20, 28, RAYWHITE);
        DrawText("← skip   → like   SPACE play/pause", 24, 58, 18, Color{200,200,200,255});

        // Card background
        DrawRectangleRounded(cardRect, 0.06f, 12, Color{ 32, 32, 36, 255 });
        DrawRectangleRoundedLines(cardRect, 0.06f, 12, 2, Color{ 80, 80, 90, 255 });

        // Fake cover block
        Rectangle coverRect = { cardRect.x + 24, cardRect.y + 24, 240, 240 };
        DrawRectangleRounded(coverRect, 0.04f, 8, Color{ 64, 64, 72, 255 });
        DrawText("COVER", (int)(coverRect.x + 78), (int)(coverRect.y + 104), 32, Color{160,160,170,255});

        // Text metadata
        int textX = (int)(coverRect.x + coverRect.width + 24);
        int textY = (int)(coverRect.y);
        DrawText(title.c_str(),  textX, textY, 26, RAYWHITE);
        DrawText(artist.c_str(), textX, textY + 36, 22, Color{ 210,210,210,255 });
        DrawText(genre.c_str(),  textX, textY + 70, 20, Color{ 180,180,190,255 });

        // “Why this track?” chip (placeholder)
        Rectangle chip = { cardRect.x + 24, cardRect.y + cardRect.height - 60, 200, 36 };
        DrawRectangleRounded(chip, 0.5f, 8, Color{ 50, 97, 210, 255 });
        DrawText("Why this track? tempo ~110", (int)chip.x + 10, (int)chip.y + 9, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
