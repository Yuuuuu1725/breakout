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
    ResetGame();
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
    brickRows = currentLevel;
    destroyed.assign(brickRows, std::vector<bool>(8, false));

    powerUps.clear();
    particles.clear();

    isSlowed = false;
    hasPaddleExtend = false;
    hasMultiBall = false;
    hasSlowBall = false;

    paddleExtendTimer = 0;
    multiBallTimer = 0;
    slowBallTimer = 0;

    currentState = MENU;
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

// 砖块破碎粒子
void Game::SpawnParticles(Vector2 pos, Color color) {
    for (int i = 0; i < 10; i++) {
        float vx = (float)(GetRandomValue(-50, 50)) / 10.0f;
        float vy = (float)(GetRandomValue(-50, 50)) / 10.0f;
        particles.push_back({ pos, {vx, vy}, color, 0.5f });
    }
}

// 道具掉落（30%概率，符合课件）
void Game::SpawnPowerUp(Vector2 pos) {
    if (GetRandomValue(0, 99) < 30) {
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

// 多球：最多3个球
void Game::AddMultiBall() {
    if (balls.size() >= 3) return;
    Vector2 p = balls[0].pos;
    balls.push_back({ p, {-3.0f, -6.0f}, 10.0f, true });
    balls.push_back({ p, {3.0f, -6.0f}, 10.0f, true });
}

void Game::SlowDownBalls() {
    if (isSlowed) return;
    isSlowed = true;
    for (auto& b : balls) {
        b.speed.x *= 0.7f;
        b.speed.y *= 0.7f;
    }
}

void Game::RestoreBallSpeed() {
    if (!isSlowed) return;
    isSlowed = false;
    for (auto& b : balls) {
        b.speed.x /= 0.7f;
        b.speed.y /= 0.7f;
    }
}

void Game::Run() {
    InitWindow(screenWidth, screenHeight, "Breakout - 道具系统+粒子");
    SetTargetFPS(60);

    Rectangle lvButtons[] = {
        {250,200,300,40}, {250,250,300,40},
        {250,300,300,40}, {250,350,300,40}, {250,400,300,40}
    };

    bool isWin = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        if (currentState == PLAYING) {
            // ========== 道具计时恢复 ==========
            if (paddleExtendTimer > 0) paddleExtendTimer -= dt;
            else if (hasPaddleExtend) { hasPaddleExtend = false; RestorePaddleSize(); }

            if (multiBallTimer > 0) multiBallTimer -= dt;
            else if (hasMultiBall) hasMultiBall = false;

            if (slowBallTimer > 0) slowBallTimer -= dt;
            else if (hasSlowBall) { hasSlowBall = false; RestoreBallSpeed(); }

            // ========== 道具移动 ==========
            for (auto& pu : powerUps) pu.pos.y += pu.speed;

            // ========== 粒子更新 ==========
            for (auto& p : particles) {
                p.pos.x += p.vel.x;
                p.pos.y += p.vel.y;
                p.life -= dt;
            }
            for (auto it = particles.begin(); it != particles.end();)
                if (it->life <= 0) it = particles.erase(it); else ++it;

            // ========== 道具拾取 ==========
            for (auto it = powerUps.begin(); it != powerUps.end();) {
                bool collide = CheckCollisionCircleRec(it->pos, 8, paddle);
                if (collide) {
                    auto effect = CreatePowerUp(it->type);
                    if (effect) effect->Apply(*this);
                }
                if (collide || it->pos.y > screenHeight)
                    it = powerUps.erase(it);
                else ++it;
            }

            // ========== 挡板移动 ==========
            if (IsKeyDown(KEY_LEFT)  && paddle.x > 0) paddle.x -= paddleSpeed;
            if (IsKeyDown(KEY_RIGHT) && paddle.x + paddle.width < screenWidth) paddle.x += paddleSpeed;

            // 返回菜单
            if (CheckCollisionPointRec(mouse, backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentState = MENU;
            }

            // ========== 球逻辑 ==========
            for (auto& ball : balls) {
                if (!ball.active) continue;
                ball.pos.x += ball.speed.x;
                ball.pos.y += ball.speed.y;

                // 左右墙
                if (ball.pos.x - ball.radius <= 0) {
                    ball.pos.x = ball.radius;
                    ball.speed.x *= -1;
                    SpawnParticles(ball.pos, SKYBLUE);
                }
                if (ball.pos.x + ball.radius >= screenWidth) {
                    ball.pos.x = screenWidth - ball.radius;
                    ball.speed.x *= -1;
                    SpawnParticles(ball.pos, SKYBLUE);
                }

                // 顶墙
                if (ball.pos.y - ball.radius <= 0) {
                    ball.pos.y = ball.radius;
                    ball.speed.y *= -1;
                    SpawnParticles(ball.pos, SKYBLUE);
                }

                // 挡板碰撞
                if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle) && ball.speed.y > 0) {
                    ball.speed.y *= -1;
                }

                // 砖块碰撞
                for (int i = 0; i < brickRows; i++) {
                    for (int j = 0; j < 8; j++) {
                        if (destroyed[i][j]) continue;
                        Rectangle br = { 110 + j*77.0f, 90 + i*37.0f, 65, 25 };
                        if (CheckCollisionCircleRec(ball.pos, ball.radius, br)) {
                            ball.speed.y *= -1;
                            destroyed[i][j] = true;
                            score += 10 * (brickRows - i);
                            SpawnParticles({ br.x + 32, br.y + 12 }, RED);
                            SpawnPowerUp({ br.x + 32, br.y + 12 });
                        }
                    }
                }

                // 球出界
                if (ball.pos.y - ball.radius > screenHeight) {
                    ball.active = false;
                }
            }

            // ========== 生命判断 ==========
            int activeBalls = 0;
            for (auto& b : balls) if (b.active) activeBalls++;

            if (activeBalls == 0) {
                if (!infiniteLives) lives--;
                if (lives <= 0 && !infiniteLives) {
                    currentState = GAME_OVER;
                    isWin = false;
                } else {
                    ResetBall();
                }
            }

            // 胜利
            if (CheckAllBricksDestroyed()) {
                isWin = true;
                currentState = GAME_OVER;
            }
        }

        // ========== 菜单 ==========
        if (currentState == MENU) {
            if (CheckCollisionPointRec(mouse, checkBoxRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                infiniteLives = !infiniteLives;
            }
            for (int i = 0; i < 5; i++) {
                if (CheckCollisionPointRec(mouse, lvButtons[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentLevel = i + 1;
                    ResetGame();
                    currentState = PLAYING;
                }
            }
        }

        // 结束界面
        if (currentState == GAME_OVER && IsKeyPressed(KEY_ENTER)) {
            currentState = MENU;
        }

        // ========== 绘制 ==========
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == MENU) {
            DrawText("BRICK GAME", 260, 100, 50, YELLOW);
            for (int i = 0; i < 5; i++) {
                DrawRectangleRec(lvButtons[i], CheckCollisionPointRec(mouse, lvButtons[i]) ? DARKBLUE : BLUE);
                DrawText(TextFormat("LEVEL %d", i+1), 340, lvButtons[i].y+5, 30, WHITE);
            }
            DrawRectangleRec(checkBoxRect, LIGHTGRAY);
            if (infiniteLives) DrawRectangle(checkBoxRect.x+5, checkBoxRect.y+5, 20, 20, GREEN);
            DrawText("Infinite Lives", checkBoxRect.x+40, checkBoxRect.y+5, 20, WHITE);
        }
        else if (currentState == GAME_OVER) {
            isWin ? DrawText("YOU WIN!", 270, 180, 50, GREEN)
                  : DrawText("GAME OVER", 270, 180, 50, RED);
            DrawText(TextFormat("SCORE: %d", score), 320, 260, 30, YELLOW);
            DrawText("PRESS ENTER TO MENU", 240, 330, 25, WHITE);
        }
        else if (currentState == PLAYING) {
            // 球
            for (auto& b : balls) if (b.active) DrawCircleV(b.pos, b.radius, WHITE);
            // 挡板
            DrawRectangleRec(paddle, SKYBLUE);
            // 砖块
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
            // 道具
            for (auto& pu : powerUps) {
                DrawCircle(pu.pos.x, pu.pos.y, 8, DARKPURPLE);
                DrawText("?", (int)pu.pos.x-4, (int)pu.pos.y-8, 18, WHITE);
            }
            // 粒子
            for (auto& p : particles) DrawCircleV(p.pos, 2, p.color);
            // UI
            DrawText(TextFormat("SCORE: %d", score), 20,20,30,YELLOW);
            DrawText(TextFormat("LIVES: %d", lives), 220,20,30,RED);
            DrawText(TextFormat("LEVEL: %d", currentLevel), 400,20,30,SKYBLUE);
            DrawRectangleRec(backButton, CheckCollisionPointRec(mouse,backButton)? DARKBLUE:BLUE);
            DrawText("MENU", backButton.x+12, backButton.y+8,20,WHITE);
            // 状态条
            DrawRectangle(0,560,800,40,Fade(DARKGRAY,0.8f));
            DrawRectangleLines(0,560,800,40,WHITE);
            hasPaddleExtend ? DrawText("Paddle",120,568,20,SKYBLUE) : DrawText("Paddle",120,568,20,GRAY);
            hasMultiBall    ? DrawText("MultiBall",220,568,20,YELLOW) : DrawText("MultiBall",220,568,20,GRAY);
            hasSlowBall     ? DrawText("Slow",340,568,20,PURPLE) : DrawText("Slow",340,568,20,GRAY);
        }
        EndDrawing();
    }
    CloseWindow();
}
