#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "BRICK GAME");

    float ballX = 400, ballY = 300;
    float speedX = 3, speedY = 3;
    float radius = 10;

    float paddleX = 400, paddleY = 520;
    float paddleW = 120, paddleH = 20;

    int score = 0;
    int lives = 3;

    const int rows = 5;
    const int cols = 8;
    bool destroyed[rows][cols] = { false };

    Color brickColors[rows] = { BLUE, GREEN, YELLOW, ORANGE, RED };
    int brickScores[rows]   = { 50, 40, 30, 20, 10 };

    float brickW = 65, brickH = 25, gap = 12;
    float startX = 110, startY = 90;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // ==============================
        // 生命为 0 → 游戏彻底暂停
        // ==============================
        if (lives <= 0) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER", 270, 200, 50, RED);
            DrawText(TextFormat("SCORE: %d", score), 320, 300, 30, YELLOW);
            DrawText("PRESS ANY KEY TO RESTART", 240, 400, 25, LIGHTGRAY);
            EndDrawing();

            if (GetKeyPressed() != 0) {
                // 重新开始
                score = 0;
                lives = 3;
                ballX = 400;
                ballY = 300;
                speedX = 3;
                speedY = 3;
                for (int i = 0; i < rows; i++)
                    for (int j = 0; j < cols; j++)
                        destroyed[i][j] = false;
            }
            continue;
        }

        // ==============================
        // 正常游戏逻辑
        // ==============================
        ballX += speedX;
        ballY += speedY;

        if (ballX - radius <= 0 || ballX + radius >= screenWidth) speedX *= -1;
        if (ballY - radius <= 0) speedY *= -1;

        // 球掉下去
        if (ballY + radius >= screenHeight) {
            lives--;
            if (lives > 0) {
                ballX = 400;
                ballY = 300;
                speedX = 3;
                speedY = 3;
            }
        }

        // 挡板移动
        if (IsKeyDown(KEY_LEFT)) paddleX -= 6;
        if (IsKeyDown(KEY_RIGHT)) paddleX += 6;

        // 挡板碰撞
        if (CheckCollisionCircleRec((Vector2){ballX, ballY}, radius,
            (Rectangle){paddleX, paddleY, paddleW, paddleH})) {
            speedY = -3;
        }

        // 砖块碰撞
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (!destroyed[i][j]) {
                    float x = startX + j*(brickW+gap);
                    float y = startY + i*(brickH+gap);
                    if (CheckCollisionCircleRec((Vector2){ballX, ballY}, radius,
                        (Rectangle){x, y, brickW, brickH})) {
                        speedY *= -1;
                        score += brickScores[i];
                        destroyed[i][j] = true;
                        goto endcol;
                    }
                }
            }
        }
        endcol:

        // 判断是否全部打完
        bool allClear = true;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (!destroyed[i][j]) allClear = false;

        if (allClear) {
            lives = 0;
        }

        // ==============================
        // 绘制游戏画面
        // ==============================
        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircle(ballX, ballY, radius, WHITE);
        DrawRectangle(paddleX, paddleY, paddleW, paddleH, SKYBLUE);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (!destroyed[i][j]) {
                    float x = startX + j*(brickW+gap);
                    float y = startY + i*(brickH+gap);
                    DrawRectangle(x, y, brickW, brickH, brickColors[i]);
                }
            }
        }

        DrawText(TextFormat("SCORE %d", score), 20, 20, 30, YELLOW);
        DrawText(TextFormat("LIFE %d", lives), 250, 20, 30, RED);

        DrawText("50", 420, 20, 30, BLUE);
        DrawText("40", 490, 20, 30, GREEN);
        DrawText("30", 560, 20, 30, YELLOW);
        DrawText("20", 630, 20, 30, ORANGE);
        DrawText("10", 700, 20, 30, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}