#include "Game.h"

void ExtendPaddleEffect::Apply(Game& game) {
    game.hasPaddleExtend = true;
    game.paddleExtendTimer = game.POWERUP_DURATION;
    game.ExtendPaddle(1.5f);
}

void MultiBallEffect::Apply(Game& game) {
    game.hasMultiBall = true;
    game.multiBallTimer = game.POWERUP_DURATION;
    game.AddMultiBall();
}

void SlowBallEffect::Apply(Game& game) {
    game.hasSlowBall = true;
    game.slowBallTimer = game.POWERUP_DURATION;
    game.SlowDownBalls();
}

std::unique_ptr<PowerUpEffect> CreatePowerUp(PowerUpType type) {
    switch (type) {
        case PowerUpType::PADDLE_EXTEND:
            return std::make_unique<ExtendPaddleEffect>();
        case PowerUpType::MULTI_BALL:
            return std::make_unique<MultiBallEffect>();
        case PowerUpType::SLOW_BALL:
            return std::make_unique<SlowBallEffect>();
        default:
            return nullptr;
    }
}

Game::Game() {
    checkBoxRect = { 250, 450, 30, 30 };
    backButton   = { 650, 20, 100, 30 };
    infiniteLives = false;
    currentLevel = 1;

    isLoading = false;
    loadTimer = 0.0f;
    showLoadedTimer = 0.0f;

    ResetGame();
}

void Game::GenerateRandomBricks() {
    brickRows = currentLevel;
    destroyed.assign(brickRows, std::vector<bool>(8, false));
    for (int i = 0; i < brickRows; i++) {
        for (int j = 0; j < 8; j++) {
            destroyed[i][j] = (GetRandomValue(0, 10) > 3);
        }
    }
}

void Game::ResetGame() {
    balls.clear();
    balls.push_back({
        {(float)screenWidth / 2, (float)screenHeight / 2},
        {(float)GetRandomValue(-4, 4), 6.0f},
        10.0f,
        true
    });
    if (balls.back().speed.x == 0) balls.back().speed.x = 4.0f;

    paddle.x = (float)screenWidth / 2 - 60.0f;
    paddle.y = (float)screenHeight - 80.0f;
    paddle.width = 120.0f;
    paddle.height = 20.0f;
    originalPaddleWidth = paddle.width;
    paddleSpeed = 7.0f;

    lives = 3;
    score = 0;

    GenerateRandomBricks();

    powerUps.clear();
    particles.clear();

    isSlowed = false;
    hasPaddleExtend = false;
    hasMultiBall = false;
    hasSlowBall = false;
    paddleExtendTimer = 0;
    multiBallTimer = 0;
    slowBallTimer = 0;

    currentState = PLAYING;
}

void Game::ResetBall() {
    balls.clear();
    balls.push_back({
        {(float)screenWidth / 2, (float)screenHeight / 2},
        {(float)GetRandomValue(-4, 4), 6.0f},
        10.0f,
        true
    });
}

bool Game::CheckAllBricksDestroyed() {
    for (auto& row : destroyed)
        for (bool b : row)
            if (!b) return false;
    return true;
}

void Game::SpawnParticles(Vector2 pos, Color color) {
    for (int i = 0; i < 8; i++) {
        float vx = (float)(GetRandomValue(-30, 30)) / 10.0f;
        float vy = (float)(GetRandomValue(-30, 30)) / 10.0f;
        particles.push_back({ pos, {vx, vy}, color, 0.5f });
    }
}

void Game::SpawnPowerUp(Vector2 pos) {
    if (GetRandomValue(0, 99) < 20) {
        PowerUpType t = (PowerUpType)GetRandomValue(0, 2);
        powerUps.push_back({ pos, t, 2.0f });
    }
}

void Game::ExtendPaddle(float ratio) {
    paddle.width = originalPaddleWidth * ratio;
}

void Game::RestorePaddleSize() {
    paddle.width = originalPaddleWidth;
}

void Game::AddMultiBall() {
    if (balls.size() > 1) return;
    Vector2 p = balls[0].pos;
    balls.push_back({ p, {-3.0f, -6.0f}, 10.0f, true });
    balls.push_back({ p, {3.0f, -6.0f}, 10.0f, true });
}

void Game::SlowDownBalls() {
    isSlowed = true;
    for (auto& b : balls) {
        b.speed.x *= 0.7f;
        b.speed.y *= 0.7f;
    }
}

void Game::RestoreBallSpeed() {
    for (auto& b : balls) {
        b.speed.x /= 0.7f;
        b.speed.y /= 0.7f;
    }
    isSlowed = false;
}

void Game::Run() {
    InitWindow(screenWidth, screenHeight, "Breakout");
    SetTargetFPS(60);

    Rectangle lv[] = {
        {250,200,300,40}, {250,250,300,40},
        {250,300,300,40}, {250,350,300,40}, {250,400,300,40}
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 m = GetMousePosition();

        if (IsKeyPressed(KEY_L) && !isLoading && currentState == PLAYING) {
            isLoading = true;
            loadTimer = 1.0f;
        }

        if (isLoading) {
            loadTimer -= dt;
            if (loadTimer <= 0) {
                isLoading = false;
                ResetGame();
                showLoadedTimer = 2.0f;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("LOADING...",
                screenWidth/2 - MeasureText("LOADING...", 60)/2,
                screenHeight/2 - 30, 60, YELLOW);
            EndDrawing();
            continue;
        }

        if (showLoadedTimer > 0) showLoadedTimer -= dt;

        if (currentState == PLAYING) {
            if (paddleExtendTimer > 0) paddleExtendTimer -= dt;
            else if (hasPaddleExtend) { hasPaddleExtend = false; RestorePaddleSize(); }

            if (multiBallTimer > 0) multiBallTimer -= dt;
            else if (hasMultiBall) hasMultiBall = false;

            if (slowBallTimer > 0) slowBallTimer -= dt;
            else if (hasSlowBall) { hasSlowBall = false; RestoreBallSpeed(); }

            for (auto& pu : powerUps) pu.pos.y += pu.speed;
            for (auto& p : particles) { p.pos.x += p.vel.x; p.pos.y += p.vel.y; p.life -= dt; }
            for (auto it = particles.begin(); it != particles.end();)
                if (it->life <= 0) it = particles.erase(it); else ++it;

            for (auto it = powerUps.begin(); it != powerUps.end();) {
                bool eaten = CheckCollisionCircleRec(it->pos, 8, paddle);
                if (eaten) { auto ef = CreatePowerUp(it->type); if (ef) ef->Apply(*this); }
                if (eaten || it->pos.y > screenHeight) it = powerUps.erase(it); else ++it;
            }

            if (IsKeyDown(KEY_LEFT) && paddle.x > 0) paddle.x -= paddleSpeed;
            if (IsKeyDown(KEY_RIGHT) && paddle.x + paddle.width < screenWidth) paddle.x += paddleSpeed;

            if (CheckCollisionPointRec(m, backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentState = MENU;
            }

            for (auto& ball : balls) {
                if (!ball.active) continue;
                ball.pos.x += ball.speed.x;
                ball.pos.y += ball.speed.y;

                if (ball.pos.x - ball.radius <= 0) {
                    ball.pos.x = ball.radius; ball.speed.x *= -1; SpawnParticles(ball.pos, SKYBLUE);
                }
                if (ball.pos.x + ball.radius >= screenWidth) {
                    ball.pos.x = screenWidth - ball.radius; ball.speed.x *= -1; SpawnParticles(ball.pos, SKYBLUE);
                }
                if (ball.pos.y - ball.radius <= 0) {
                    ball.pos.y = ball.radius; ball.speed.y *= -1; SpawnParticles(ball.pos, SKYBLUE);
                }

                if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle) && ball.speed.y > 0) {
                    ball.speed.y *= -1;
                }

                for (int i = 0; i < brickRows; i++) {
                    for (int j = 0; j < 8; j++) {
                        if (destroyed[i][j]) continue;
                        Rectangle br = { 110 + j*77.0f, 90 + i*37.0f, 65, 25 };
                        if (CheckCollisionCircleRec(ball.pos, ball.radius, br)) {
                            ball.speed.y *= -1;
                            destroyed[i][j] = true;
                            int pts = 10 * (brickRows - i);
                            score += pts;
                            SpawnParticles({br.x+32, br.y+12}, RED);
                            SpawnPowerUp({br.x+32, br.y+12});
                        }
                    }
                }

                if (ball.pos.y - ball.radius > screenHeight) {
                    ball.active = false;
                }
            }

            int active = 0;
            for (auto& b : balls) if (b.active) active++;
            if (active == 0) {
                if (!infiniteLives) lives--;
                if (lives <= 0 && !infiniteLives) {
                    currentState = GAME_OVER;
                } else {
                    ResetBall();
                }
            }

            if (CheckAllBricksDestroyed()) {
                currentState = GAME_OVER;
            }
        }

        if (currentState == MENU) {
            if (CheckCollisionPointRec(m, checkBoxRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                infiniteLives = !infiniteLives;
            }
            for (int i = 0; i < 5; i++) {
                if (CheckCollisionPointRec(m, lv[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentLevel = i+1;
                    ResetGame();
                    currentState = PLAYING;
                }
            }
        }

        if (currentState == GAME_OVER && IsKeyPressed(KEY_ENTER)) {
            currentState = MENU;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == MENU) {
            DrawText("BRICK GAME", 260, 100, 50, YELLOW);
            for (int i = 0; i < 5; i++) {
                DrawRectangleRec(lv[i], CheckCollisionPointRec(m, lv[i]) ? DARKBLUE : BLUE);
                DrawText(TextFormat("LEVEL %d", i+1), 340, lv[i].y+5, 30, WHITE);
            }
            DrawRectangleRec(checkBoxRect, LIGHTGRAY);
            if (infiniteLives) DrawRectangle(checkBoxRect.x+5, checkBoxRect.y+5, 20,20, GREEN);
            DrawText("Infinite Lives", checkBoxRect.x+40, checkBoxRect.y+5, 20, WHITE);
        }
        else if (currentState == GAME_OVER) {
            DrawText("GAME OVER", 270, 180, 50, RED);
            DrawText(TextFormat("SCORE: %d", score), 320, 260, 30, YELLOW);
            DrawText("PRESS ENTER TO MENU", 240, 330, 25, WHITE);
        }
        else if (currentState == PLAYING) {
            for (auto& b : balls) if (b.active) DrawCircleV(b.pos, b.radius, WHITE);
            DrawRectangleRec(paddle, SKYBLUE);

            for (int i = 0; i < brickRows; i++) {
                for (int j = 0; j < 8; j++) {
                    if (destroyed[i][j]) continue;
                    float x = 110 + j*77.0f;
                    float y = 90 + i*37.0f;
                    int pts = 10*(brickRows-i);
                    Color c;
                    if (pts == 10) c = BLUE;
                    else if (pts == 20) c = GREEN;
                    else if (pts == 30) c = YELLOW;
                    else if (pts == 40) c = ORANGE;
                    else c = RED;
                    DrawRectangle(x,y,65,25,c);
                    const char* t = TextFormat("%d", pts);
                    int tw = MeasureText(t,16);
                    DrawText(t, x+32-tw/2, y+5, 16, BLACK);
                }
            }

            for (auto& pu : powerUps) {
                DrawCircle(pu.pos.x, pu.pos.y, 8, DARKPURPLE);
                DrawText("?", (int)pu.pos.x-4, (int)pu.pos.y-8, 18, WHITE);
            }

            for (auto& p : particles) DrawCircleV(p.pos, 2, p.color);

            DrawText(TextFormat("SCORE: %d", score), 20,20,30,YELLOW);
            DrawText(TextFormat("LIVES: %d", lives), 220,20,30,RED);
            DrawText(TextFormat("LEVEL: %d", currentLevel), 400,20,30,SKYBLUE);

            DrawRectangleRec(backButton, CheckCollisionPointRec(m,backButton)? DARKBLUE:BLUE);
            DrawText("MENU", backButton.x+12, backButton.y+8,20,WHITE);

            DrawRectangle(0,560,800,40,Fade(DARKGRAY,0.8f));
            DrawRectangleLines(0,560,800,40,WHITE);
            hasPaddleExtend ? DrawText("Paddle",    120,568,20,SKYBLUE) : DrawText("Paddle",   120,568,20,GRAY);
            hasMultiBall    ? DrawText("MultiBall", 220,568,20,YELLOW) : DrawText("MultiBall",220,568,20,GRAY);
            hasSlowBall     ? DrawText("Slow",      340,568,20,PURPLE) : DrawText("Slow",     340,568,20,GRAY);

            if (showLoadedTimer > 0) {
                DrawText("RESOURCE LOADED!", 250, 500, 30, GREEN);
            }
        }

        EndDrawing();
    }
    CloseWindow();
}
