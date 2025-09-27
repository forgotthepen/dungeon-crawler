#include <cmath>
#include <iostream>
#include <string>
#include <raylib.h>
#include "dungeon.h"

const int tileSize = 20;
const float playerRadius = tileSize / 2.0f;
const int playerSpeedStep = 20;
const int playerSpeedMax = 1000;
const int debugTextStep = 12;
const int targetFps = 40;

int screenWidth = 1280;
int screenHeight = 720;
int playerSpeed = 300;
bool showFPS = true;
bool showGrid = true;
bool isFullscreen = false;
bool pendingScreenResize = false;

int main() {

    InitWindow(screenWidth, screenHeight, "Dungeon Crawler - Procedural Generation");

    Dungeon dungeon(screenWidth / tileSize, screenHeight / tileSize);
    dungeon.generate();

    Vector2 playerPos = dungeon.getPlayerSpawn(tileSize);

    SetTargetFPS(targetFps);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- input handling ---
        if (IsKeyPressed(KEY_F1)) showFPS = !showFPS;
        if (IsKeyPressed(KEY_F2)) showGrid = !showGrid;
        if (IsKeyPressed(KEY_MINUS) && playerSpeed >= playerSpeedStep) playerSpeed -= playerSpeedStep;
        if (IsKeyPressed(KEY_EQUAL) && playerSpeed < playerSpeedMax) playerSpeed += playerSpeedStep;
        if (IsKeyPressed(KEY_F11)) {
            if (isFullscreen) {
                ClearWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
            } else {
                SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
            }
            isFullscreen = !isFullscreen;
            ToggleFullscreen();

            // update screen size
            screenWidth = GetRenderWidth();
            screenHeight = GetRenderHeight();
            screenHeight = GetRenderHeight();

            pendingScreenResize = true;
        }
        
        if (pendingScreenResize && (GetRenderWidth() != screenWidth || GetRenderHeight() != screenHeight)) {
            pendingScreenResize = false;

            // update screen size
            screenWidth = GetRenderWidth();
            screenHeight = GetRenderHeight();

            // update dungeon dimensions + regenerate
            dungeon.resize(screenWidth / tileSize, screenHeight / tileSize);
            dungeon.generate(dungeon.getSeed());
            playerPos = dungeon.getPlayerSpawn(tileSize);
        }

        // Movement input
        int newMoveX = 0;
        int newMoveY = 0;
        if (IsKeyDown(KEY_W)) newMoveY -= 1;
        if (IsKeyDown(KEY_S)) newMoveY += 1;
        if (IsKeyDown(KEY_A)) newMoveX -= 1;
        if (IsKeyDown(KEY_D)) newMoveX += 1;

        if (0 != newMoveX) {
            const float newPlayPosX = playerPos.x + newMoveX * playerSpeed * dt;

            const int tileIdxX = (int)(newPlayPosX / tileSize);
            const int tileIdxY = (int)(playerPos.y / tileSize);
            if (dungeon.isWalkable(tileIdxX, tileIdxY)) {
                playerPos.x = newPlayPosX;
            }
        }

        if (0 != newMoveY) {
            const float newPlayPosY = playerPos.y + newMoveY * playerSpeed * dt;

            const int tileIdxX = (int)(playerPos.x / tileSize);
            const int tileIdxY = (int)(newPlayPosY / tileSize);
            if (dungeon.isWalkable(tileIdxX, tileIdxY)) {
                playerPos.y = newPlayPosY;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        // dungeon
        dungeon.draw([](int posX, int posY) {
            DrawRectangle(posX * tileSize, posY * tileSize, tileSize, tileSize, DARKGRAY);
        });
        // player
        DrawCircleV(playerPos, playerRadius, BLUE);

        // --- debug overlays ---
        if (showGrid) {
            Color gridColor = Color{100, 100, 100, 60}; // faint gray
            // Vertical dashed lines
            for (int x = 0; x <= screenWidth; x += tileSize) {
                for (int y = 0; y < screenHeight; y += 8) {
                    DrawLine(x, y, x, y + 4, gridColor);
                }
            }
            // Horizontal dashed lines
            for (int y = 0; y <= screenHeight; y += tileSize) {
                for (int x = 0; x < screenWidth; x += 8) {
                    DrawLine(x, y, x + 4, y, gridColor);
                }
            }
        }

        if (showFPS) {
            // DrawFPS(10, 10);
            int y_pos = 10;
            auto text = std::string("FPS: ") + std::to_string(GetFPS());
            DrawText(text.c_str(), 5, y_pos, 10, ORANGE);
            y_pos += debugTextStep;

            text = std::string("Dungeon: ") + std::to_string(dungeon.getWidth()) + " x " + std::to_string(dungeon.getHeight()) + " tiles";
            DrawText(text.c_str(), 5, y_pos, 10, ORANGE);
            y_pos += debugTextStep;

            text = std::string("Screen: ") + std::to_string(screenWidth) + " x " + std::to_string(screenHeight);
            DrawText(text.c_str(), 5, y_pos, 10, ORANGE);
            y_pos += debugTextStep;

            text = std::string("Speed: ") + std::to_string(playerSpeed);
            DrawText(text.c_str(), 5, y_pos, 10, ORANGE);
            y_pos += debugTextStep;

            text = std::string("Grid: ") + (showGrid ? "On" : "Off");
            DrawText(text.c_str(), 5, y_pos, 10, ORANGE);
            y_pos += debugTextStep;

            text = std::string("Fullscreen: ") + (isFullscreen ? "On" : "Off");
            DrawText(text.c_str(), 5, y_pos, 10, ORANGE);
            y_pos += debugTextStep;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
