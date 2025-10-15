#include "raylib.h"
#include <string>
#include <vector>
#include <fstream>
#include "spotify_bridge.hpp"

// ---- Local secrets header (NOT in Git): create app/spotify_config.h locally with your keys
//   #pragma once
//   #define SPOTIFY_CLIENT_ID "your_id"
//   #define SPOTIFY_CLIENT_SECRET "your_secret"
#include "spotify_config.h"  // this file lives only on your Mac

static bool write_bytes(const std::string& path, const std::vector<unsigned char>& data) {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write(reinterpret_cast<const char*>(data.data()), (std::streamsize)data.size());
    return (bool)f;
}

// --- UI helpers ---------------------------------------------------------------
struct AppState {
    bool playing = false;
    bool showLike = false;
    bool showSkip = false;
    float overlayTimer = 0.0f;
    bool started = false;

    // Spotify
    std::string tokenErr;
    std::string searchErr;
    std::string dlErr;

    std::string title = "Track Title";
    std::string artist = "Artist Name";
    std::string previewURL;   // may be empty
    std::string tmpAudioPath; // local cache file we write (mp3)
};

static void DrawPlayPauseIcon(Vector2 center, float r, bool playing) {
    DrawCircleV(center, r, Color{55, 120, 230, 255});
    if (playing) {
        float w = r * 0.35f, h = r * 0.9f, gap = r * 0.25f;
        Rectangle leftBar  = { center.x - gap - w, center.y - h/2, w, h };
        Rectangle rightBar = { center.x + gap,     center.y - h/2, w, h };
        DrawRectangleRounded(leftBar,  0.2f, 4, RAYWHITE);
        DrawRectangleRounded(rightBar, 0.2f, 4, RAYWHITE);
    } else {
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
    InitWindow(screenWidth, screenHeight, "Swipe Sonic — Spotify Preview");
    SetTargetFPS(60);

    InitAudioDevice(); SetMasterVolume(1.0f);

    AppState S;

    // ----- 1) Get Spotify token
    {
        std::string err;
        S.tokenErr.clear();
        std::string token = spotify_fetch_access_token(SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, err);
        if (token.empty()) {
            S.tokenErr = "Token error: " + err;
        } else {
            // ----- 2) Search (change the query string if you like)
            std::string title, artist, purl, sErr;
            bool ok = spotify_search_first_preview(token, "lofi beats", title, artist, purl, sErr);
            if (!ok) {
                S.searchErr = "Search error: " + sErr;
            } else {
                S.title = title; S.artist = artist; S.previewURL = purl;

                // ----- 3) Download preview (if available)
                if (!S.previewURL.empty()) {
                    std::vector<unsigned char> bytes;
                    std::string dErr;
                    if (http_download_bytes(S.previewURL, bytes, dErr)) {
                        S.tmpAudioPath = "preview_tmp.mp3";
                        if (!write_bytes(S.tmpAudioPath, bytes)) {
                            S.dlErr = "Write failed";
                        }
                    } else {
                        S.dlErr = "Download error: " + dErr;
                    }
                }
            }
        }
    }

    // Prepare music if we got a file
    Music music = {0};
    bool musicOk = false;
    if (!S.tmpAudioPath.empty() && FileExists(S.tmpAudioPath.c_str())) {
        music = LoadMusicStream(S.tmpAudioPath.c_str());
        musicOk = (music.stream.buffer != nullptr);
    }

    // Card geometry
    const int cardW = 520, cardH = 360;
    Rectangle cardRect{ (float)(screenWidth - cardW)/2.0f, (float)(screenHeight - cardH)/2.0f, (float)cardW, (float)cardH };
    const float OVERLAY_DURATION = 0.55f;

    while (!WindowShouldClose()) {
        // INPUT
        if (IsKeyPressed(KEY_SPACE)) {
            S.playing = !S.playing;
            if (musicOk) {
                if (S.playing) { if (!S.started) { PlayMusicStream(music); S.started = true; } else { ResumeMusicStream(music); } }
                else           { PauseMusicStream(music); }
            }
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { S.showLike=true; S.showSkip=false; S.overlayTimer=OVERLAY_DURATION; }
        if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) { S.showSkip=true; S.showLike=false; S.overlayTimer=OVERLAY_DURATION; }
        if (IsKeyPressed(KEY_R) && musicOk) { StopMusicStream(music); PlayMusicStream(music); S.started=true; S.playing=true; }

        if (musicOk && S.playing) {
            UpdateMusicStream(music);
            float len = GetMusicTimeLength(music);
            float pos = GetMusicTimePlayed(music);
            if (len > 0 && pos >= len - 0.01f) { S.playing=false; StopMusicStream(music); }
        }

        if (S.overlayTimer > 0.0f) {
            S.overlayTimer -= GetFrameTime();
            if (S.overlayTimer <= 0.0f) { S.showLike = S.showSkip = false; }
        }

        // DRAW
        BeginDrawing();
        ClearBackground(Color{18,18,20,255});

        DrawText("Swipe Sonic", 24, 20, 28, RAYWHITE);
        DrawText("← skip   → like   SPACE play/pause   R restart", 24, 58, 18, Color{200,200,200,255});

        DrawRectangleRounded(cardRect, 0.06f, 12, Color{32,32,36,255});
        DrawRectangleRoundedLines(cardRect, 0.06f, 12, 2, Color{80,80,90,255});

        Rectangle coverRect{ cardRect.x+24, cardRect.y+24, 240, 240 };
        DrawRectangleRounded(coverRect, 0.04f, 8, (S.playing ? Color{70,90,120,255} : Color{64,64,72,255}));
        DrawText("COVER", (int)(coverRect.x+78), (int)(coverRect.y+104), 32, Color{160,160,170,255});

        int textX = (int)(coverRect.x + coverRect.width + 24);
        int textY = (int)(coverRect.y);
        DrawText(S.title.c_str(),  textX, textY, 26, RAYWHITE);
        DrawText(S.artist.c_str(), textX, textY + 36, 22, Color{210,210,210,255});
        DrawText("Source: Spotify", textX, textY + 70, 20, Color{180,180,190,255});

        Vector2 iconC{ cardRect.x + cardRect.width - 44.0f, cardRect.y + 44.0f };
        DrawPlayPauseIcon(iconC, 20.0f, S.playing);

        Rectangle chip{ cardRect.x + 24, cardRect.y + cardRect.height - 60, 300, 36 };
        DrawRectangleRounded(chip, 0.5f, 8, Color{50,97,210,255});
        DrawText("Why this track? preview via Spotify", (int)chip.x + 10, (int)chip.y + 9, 16, RAYWHITE);

        // Status lines
        const char* status = S.playing ? "Playing" : "Paused";
        DrawText(TextFormat("Status: %s", status), 24, screenHeight - 86, 18, Color{200,200,200,255});

        if (!S.tokenErr.empty()) DrawText(S.tokenErr.c_str(), 24, screenHeight - 64, 18, RED);
        else if (!S.searchErr.empty()) DrawText(S.searchErr.c_str(), 24, screenHeight - 64, 18, RED);
        else if (S.previewURL.empty()) DrawText("No preview available for this track", 24, screenHeight - 64, 18, ORANGE);
        else DrawText(TextFormat("Preview: %s", S.previewURL.c_str()), 24, screenHeight - 64, 18, Color{200,200,200,255});

        if (!S.dlErr.empty()) DrawText(S.dlErr.c_str(), 24, screenHeight - 42, 18, RED);

        // Progress bar if music OK
        if (musicOk) {
            float len = GetMusicTimeLength(music);
            float pos = GetMusicTimePlayed(music);
            float pct = (len > 0.0f) ? pos/len : 0.0f;
            Rectangle barBG = { cardRect.x + 24, cardRect.y + cardRect.height - 100, cardRect.width - 48, 10 };
            DrawRectangleRounded(barBG, 0.5f, 8, Color{ 60, 60, 70, 255 });
            Rectangle barFG = barBG; barFG.width = barBG.width * pct;
            DrawRectangleRounded(barFG, 0.5f, 8, Color{ 80, 180, 110, 255 });
            DrawText(TextFormat("%0.1fs / %0.1fs", pos, len), (int)barBG.x, (int)barBG.y - 22, 18, Color{190,190,200,255});
        }

        if (S.overlayTimer > 0.0f) {
            Color veil = S.showLike ? Color{10,170,80,120} : Color{200,40,40,120};
            DrawRectangle(0,0,screenWidth,screenHeight, veil);
            const char* msg = S.showLike ? "LIKED" : "SKIPPED";
            int font = 64; int w = MeasureText(msg, font);
            DrawText(msg, (screenWidth - w)/2, (screenHeight - font)/2, font, RAYWHITE);
        }

        EndDrawing();
    }

    if (musicOk) UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
