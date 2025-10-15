#include "raylib.h"
#include <string>

// --- Simple app state (no audio yet) -----------------------------------------
struct AppState {
    bool playing = false;      // SPACE toggles
    bool showLike = false;     // RIGHT/D shows "LIKED" overlay briefly
    bool showSkip = false;     // LEFT/A shows "SKIPPED" overlay briefly
    float overlayTimer = 0.0f; // seconds remaining for overlay
};

static void DrawPlayPauseIcon(Vector2 center, float r, bool playing) {
    // Circle background
    DrawCircleV(center, r, Color{55, 120, 230, 255});
    if (playing) {
        // Pause: two vertical bars
        float w = r * 0.35f, h = r * 0.9f, gap = r * 0.25f;
        Rectangle leftBar  = { center.x - gap - w, center.y - h/2, w, h };
        Rectangle rightBar = { center.x + gap,     center.y - h/2, w, h };
        DrawRectangleRounded(leftBar,  0.2f, 4, RAYWHITE);
        DrawRectangleRounded(rightBar, 0.2f, 4, RAYWHITE);
    } else {
        // Play: triangle
        Vector2 p1{ center.x - r*0.35f, center.y - r*0.6f };
        Vector2 p2{ center.x - r*0.35f, center.y + r*0.6f };
        Vector2 p3{ center.x + r*0.65f, center.y };
        DrawTriangle(p1, p2, p3, RAYWHITE);
    }
}

int main() {
    const int screenWidth = 900;
    const int screenHeight = 600;

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Swipe Sonic — Dev Shell");
    SetTargetFPS(60);

    AppState S;

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

    const float OVERLAY_DURATION = 0.55f; // seconds

    while (!WindowShouldClose()) {
        // --- INPUT ------------------------------------------------------------
        if (IsKeyPressed(KEY_SPACE)) {
            S.playing = !S.playing;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            S.showLike = true;  S.showSkip = false;  S.overlayTimer = OVERLAY_DURATION;
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            S.showSkip = true;  S.showLike = false;  S.overlayTimer = OVERLAY_DURATION;
        }

        // Overlay timer countdown
        if (S.overlayTimer > 0.0f) {
            S.overlayTimer -= GetFrameTime();
            if (S.overlayTimer <= 0.0f) { S.showLike = S.showSkip = false; }
        }

        // --- DRAW -------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Color{ 18, 18, 20, 255 });

        // Header / hotkeys
        DrawText("Swipe Sonic", 24, 20, 28, RAYWHITE);
        DrawText("← skip   → like   SPACE play/pause", 24, 58, 18, Color{200,200,200,255});

        // Card background
        DrawRectangleRounded(cardRect, 0.06f, 12, Color{ 32, 32, 36, 255 });
        DrawRectangleRoundedLines(cardRect, 0.06f, 12, 2, Color{ 80, 80, 90, 255 });

        // Fake cover block
        Rectangle coverRect = { cardRect.x + 24, cardRect.y + 24, 240, 240 };
        DrawRectangleRounded(coverRect, 0.04f, 8, S.playing ? Color{ 70, 90, 120, 255 } : Color{ 64, 64, 72, 255 });
        DrawText("COVER", (int)(coverRect.x + 78), (int)(coverRect.y + 104), 32, Color{160,160,170,255});

        // Text metadata
        int textX = (int)(coverRect.x + coverRect.width + 24);
        int textY = (int)(coverRect.y);
        DrawText(title.c_str(),  textX, textY, 26, RAYWHITE);
        DrawText(artist.c_str(), textX, textY + 36, 22, Color{ 210,210,210,255 });
        DrawText(genre.c_str(),  textX, textY + 70, 20, Color{ 180,180,190,255 });

        // Play/Pause icon in top-right of card
        Vector2 iconC = { cardRect.x + cardRect.width - 44.0f, cardRect.y + 44.0f };
        DrawPlayPauseIcon(iconC, 20.0f, S.playing);

        // “Why this track?” chip (placeholder)
        Rectangle chip = { cardRect.x + 24, cardRect.y + cardRect.height - 60, 260, 36 };
        DrawRectangleRounded(chip, 0.5f, 8, Color{ 50, 97, 210, 255 });
        DrawText("Why this track? tempo ~110", (int)chip.x + 10, (int)chip.y + 9, 16, RAYWHITE);

        // Status line
        const char* status = S.playing ? "Playing" : "Paused";
        DrawText(TextFormat("Status: %s", status), 24, screenHeight - 34, 18, Color{200,200,200,255});

        // Like/Skip transient overlay
        if (S.overlayTimer > 0.0f) {
            Color veil = S.showLike ? Color{ 10, 170, 80, 120 } : Color{ 200, 40, 40, 120 };
            DrawRectangle(0, 0, screenWidth, screenHeight, veil);

            const char* msg = S.showLike ? "LIKED" : "SKIPPED";
            int font = 64;
            int w = MeasureText(msg, font);
            DrawText(msg, (screenWidth - w)/2, (screenHeight - font)/2, font, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

