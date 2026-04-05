#include "Game.h"

Game::Game() {
    currentLevel = 1;
    ResetGame();
}

void Game::ResetGame() {
    ballPos.x = (float)screenWidth / 2.0f;
    ballPos.y = (float)screenHeight / 2.0f;

    ballSpeed.x = (float)GetRandomValue(-5, 5);
    if (ballSpeed.x == 0) ballSpeed.x = 4;
    ballSpeed.y = 6.0f;

    radius = 10.0f;

    paddle.x = (float)screenWidth / 2.0f - 60.0f;
    paddle.y = (float)screenHeight - 80.0f;
    paddle.width = 120.0f;
    paddle.height = 20.0f;
    paddleSpeed = 7.0f;

    lives = 3;
    score = 0;
    brickScores.clear();

    if (currentLevel == 1) {
        brickRows = 1;
        brickScores.push_back(10);
    }
    else if (currentLevel == 2) {
        brickRows = 2;
        brickScores.push_back(20);
        brickScores.push_back(10);
    }
    else if (currentLevel == 3) {
        brickRows = 3;
        brickScores.push_back(30);
        brickScores.push_back(20);
        brickScores.push_back(10);
    }
    else if (currentLevel == 4) {
        brickRows = 4;
        brickScores.push_back(40);
        brickScores.push_back(30);
        brickScores.push_back(20);
        brickScores.push_back(10);
    }
    else {
        brickRows = 5;
        brickScores.push_back(50);
        brickScores.push_back(40);
        brickScores.push_back(30);
        brickScores.push_back(20);
        brickScores.push_back(10);
    }

    destroyed.assign(brickRows, std::vector<bool>(8, false));
    currentState = MENU;
}

bool Game::CheckAllBricksDestroyed() {
    for (int i = 0; i < brickRows; i++)
        for (int j = 0; j < 8; j++)
            if (!destroyed[i][j])
                return false;
    return true;
}

void Game::Run() {
    InitWindow(screenWidth, screenHeight, "BRICK GAME");
    SetTargetFPS(60);

    Rectangle btnLevel[5] = {
        {250, 200, 300, 40},
        {250, 250, 300, 40},
        {250, 300, 300, 40},
        {250, 350, 300, 40},
        {250, 400, 300, 40}
    };

    bool isWin = false;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (currentState == MENU) {
            if (CheckCollisionPointRec(mouse, checkBoxRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                infiniteLives = !infiniteLives;
            }

            for (int i = 0; i < 5; i++) {
                if (CheckCollisionPointRec(mouse, btnLevel[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentLevel = i + 1;
                    ResetGame();
                    currentState = PLAYING;
                }
            }
        }
        else if (currentState == GAME_OVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = MENU;
            }
        }
        else if (currentState == PLAYING) {
            if (CheckCollisionPointRec(mouse, backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentState = MENU;
            }

            ballPos.x += ballSpeed.x;
            ballPos.y += ballSpeed.y;

            if (ballPos.x - radius <= 0 || ballPos.x + radius >= screenWidth)
                ballSpeed.x = -ballSpeed.x;

            if (ballPos.y - radius <= 0)
                ballSpeed.y = -ballSpeed.y;

            if (IsKeyDown(KEY_LEFT) && paddle.x > 0)
                paddle.x -= paddleSpeed;
            if (IsKeyDown(KEY_RIGHT) && paddle.x + paddle.width < screenWidth)
                paddle.x += paddleSpeed;

            if (CheckCollisionCircleRec(ballPos, radius, paddle)) {
                if (ballSpeed.y > 0) {
                    ballSpeed.y = -ballSpeed.y;
                }
            }

            for (int i = 0; i < brickRows; i++) {
                for (int j = 0; j < 8; j++) {
                    if (!destroyed[i][j]) {
                        float x = 110.0f + (float)j * 77.0f;
                        float y = 90.0f + (float)i * 37.0f;
                        Rectangle r = { x, y, 65, 25 };

                        if (CheckCollisionCircleRec(ballPos, radius, r)) {
                            ballSpeed.y = -ballSpeed.y;
                            destroyed[i][j] = true;
                            score += brickScores[i];
                        }
                    }
                }
            }

            if (CheckAllBricksDestroyed()) {
                isWin = true;
                currentState = GAME_OVER;
            }

            if (ballPos.y + radius >= screenHeight) {
                if (!infiniteLives) {
                    lives--;
                }

                if (lives <= 0 && !infiniteLives) {
                    isWin = false;
                    currentState = GAME_OVER;
                } else {
                    ballPos.x = (float)screenWidth / 2.0f;
                    ballPos.y = (float)screenHeight / 2.0f;
                    ballSpeed.x = (float)GetRandomValue(-5, 5);
                    if (ballSpeed.x == 0) ballSpeed.x = 4;
                    ballSpeed.y = 6.0f;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == MENU) {
            DrawText("BRICK GAME", 260, 100, 50, YELLOW);

            for (int i = 0; i < 5; i++) {
                DrawRectangleRec(btnLevel[i], CheckCollisionPointRec(mouse, btnLevel[i]) ? DARKBLUE : BLUE);
                DrawText(TextFormat("LEVEL %d", i + 1), 340, btnLevel[i].y + 5, 30, WHITE);
            }

            DrawRectangleRec(checkBoxRect, LIGHTGRAY);
            if (infiniteLives) {
                DrawRectangle(checkBoxRect.x + 5, checkBoxRect.y + 5, 20, 20, GREEN);
            }
            DrawText("Infinite Lives", checkBoxRect.x + 40, checkBoxRect.y + 5, 20, WHITE);
        }
        else if (currentState == GAME_OVER) {
            if (isWin) {
                DrawText("YOU WIN!", 270, 180, 50, GREEN);
            } else {
                DrawText("GAME OVER", 270, 180, 50, RED);
            }

            DrawText(TextFormat("SCORE: %d", score), 320, 260, 30, YELLOW);
            DrawText("PRESS ENTER TO MENU", 240, 330, 25, WHITE);
        }
        else if (currentState == PLAYING) {
            DrawCircleV(ballPos, radius, WHITE);
            DrawRectangleRec(paddle, SKYBLUE);

            for (int i = 0; i < brickRows; i++) {
                for (int j = 0; j < 8; j++) {
                    if (!destroyed[i][j]) {
                        float x = 110.0f + (float)j * 77.0f;
                        float y = 90.0f + (float)i * 37.0f;
                        Color c;
                        if (brickScores[i] == 10) c = RED;
                        else if (brickScores[i] == 20) c = ORANGE;
                        else if (brickScores[i] == 30) c = YELLOW;
                        else if (brickScores[i] == 40) c = GREEN;
                        else c = BLUE;
                        DrawRectangle(x, y, 65, 25, c);
                    }
                }
            }

            DrawText(TextFormat("SCORE: %d", score), 20, 20, 30, YELLOW);
            DrawText(TextFormat("LIVES: %d", lives), 220, 20, 30, RED);
            DrawText(TextFormat("LEVEL: %d", currentLevel), 400, 20, 30, SKYBLUE);
            DrawText("50", 550, 20, 30, BLUE);
            DrawText("40", 600, 20, 30, GREEN);
            DrawText("30", 650, 20, 30, YELLOW);
            DrawText("20", 700, 20, 30, ORANGE);
            DrawText("10", 750, 20, 30, RED);

            DrawRectangleRec(backButton, CheckCollisionPointRec(mouse, backButton) ? DARKBLUE : BLUE);
            DrawText("BACK MENU", backButton.x + 10, backButton.y + 10, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
}