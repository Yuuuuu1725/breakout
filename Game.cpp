#include "Game.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>
using json = nlohmann::json;

Game::Game(const std::string& mapPath)
    : screenWidth(800), screenHeight(600), score(0), lives(3), state(GameState::PLAYING),
      ballStart({400, 500}), ballSpeed({0, 0}), ballRadius(10),
      paddle(350, 550, 100, 15), scorePerBrick(10),
      powerUpDropChance(0.15f), extendDuration(5.0f), slowDuration(5.0f), slowMultiplier(0.5f),
      opponentPaddle(0, 0, 100, 20), 
      isNetworked(false), isHost(false), network(nullptr),
      interpolationTimer(0.0f), INTERP_DELAY(0.05f), currentSpeedMult(1.0f), extendStackCount(0)
{
    std::ifstream f(mapPath);
    if (!f.is_open()) { std::cerr << "找不到 config.json" << std::endl; return; }

    json config = json::parse(f);
    screenWidth = config["window"]["width"];
    screenHeight = config["window"]["height"];
    lives = config["game"]["initialLives"];
    scorePerBrick = config["game"]["scorePerBrick"];
    
    ballStart = { config["ball"]["startX"], config["ball"]["startY"] };
    ballSpeed = { config["ball"]["speedX"], config["ball"]["speedY"] };
    ballRadius = config["ball"]["radius"];
    
    paddle = Paddle(config["paddle"]["startX"], config["paddle"]["startY"], config["paddle"]["width"], config["paddle"]["height"]);
    opponentPaddle = Paddle(0, paddle.GetRect().y, paddle.GetRect().width, paddle.GetRect().height);

    if (config.contains("powerups")) {
        powerUpDropChance = config["powerups"].value("dropChance", 0.3f);
        extendDuration = config["powerups"].value("extendDuration", 5.0f);
        slowDuration = config["powerups"].value("slowDuration", 5.0f);
        slowMultiplier = config["powerups"].value("slowMultiplier", 0.5f);
    }

    if (config["brick"].contains("rows")) {
        InitBricks(config["brick"]["rows"], config["brick"]["cols"], config["brick"]["startX"], config["brick"]["startY"], 
                   config["brick"]["spacingX"], config["brick"]["spacingY"], config["brick"]["width"], config["brick"]["height"], config["brick"]["rowColors"]);
    }
    
    balls.emplace_back(ballStart, Vector2{0, 0}, ballRadius);
}

void Game::InitBricks(int rows, int cols, float startX, float startY, float spacingX, float spacingY, float w, float h, const nlohmann::json& colors) {
    bricks.clear();
    for (int r = 0; r < rows; r++) {
        Color c = { colors[r][0], colors[r][1], colors[r][2], colors[r][3] };
        for (int c_idx = 0; c_idx < cols; c_idx++) {
            bricks.emplace_back(startX + c_idx * spacingX, startY + r * spacingY, w, h, c);
        }
    }
}

void Game::ExtendPaddle(float amount) { paddle.Extend(amount); }
void Game::ShrinkPaddle(float amount) { paddle.Shrink(amount); }
void Game::SetBallSpeedMult(float mult) { currentSpeedMult = mult; for (auto& b : balls) b.SetSpeedMultiplier(mult); }
void Game::ResetBallSpeedMult() { currentSpeedMult = 1.0f; for (auto& b : balls) b.SetSpeedMultiplier(1.0f); }
void Game::SpawnExtraBall() {
    if (!balls.empty()) {
        Ball newBall = balls[0];
        newBall.SetSpeedMultiplier(currentSpeedMult);
        balls.push_back(newBall);
    }
}

void Game::SpawnParticles(Vector2 pos, Color color, int count) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.position = pos;
        p.speed = { (float)(rand() % 300 - 150), (float)(rand() % 300 - 150) };
        p.color = color;
        p.life = 0.6f;
        p.size = (float)(rand() % 4 + 2);
        particles.push_back(p);
    }
}

void Game::UpdateParticles(float dt) {
    for (auto& p : particles) {
        p.position.x += p.speed.x * dt;
        p.position.y += p.speed.y * dt;
        p.speed.y += 300 * dt;
        p.life -= dt;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.life <= 0; }), particles.end());
}

void Game::HandleInput(int paddleSpeed) {
    if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft(paddleSpeed);
    if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(paddleSpeed, screenWidth);
}

void Game::UpdateBalls() {
    for (auto& ball : balls) {
        ball.Move();
        ball.BounceEdge(screenWidth, screenHeight);
        ball.BouncePaddle(paddle.GetRect());
    }
}

// 【纯净版碰撞检测】
void Game::CheckCollisions() {
    for (auto& ball : balls) {
        for (auto& brick : bricks) { // 恢复正常的遍历，不要瞎写索引 i
            if (brick.IsActive() && ball.CollideWithBrick(brick.GetRect())) {
                brick.SetActive(false);
                score += scorePerBrick;
                
                Rectangle r = brick.GetRect();
                SpawnParticles({r.x + r.width/2, r.y + r.height/2}, brick.GetColor(), 15);

                if ((float)(rand() % 100) / 100.0f < powerUpDropChance) {
                    PowerUpType types[3] = { PowerUpType::EXTEND, PowerUpType::MULTI, PowerUpType::SLOW };
                    PowerUp pu;
                    pu.rect = { r.x + 30, r.y, 30, 15 };
                    pu.type = types[rand() % 3];
                    pu.speed = 3.0f;
                    pu.active = true;
                    powerUps.push_back(pu);
                }
                break;
            }
        }
    }
}

void Game::HandleBallsLost() {
    balls.erase(std::remove_if(balls.begin(), balls.end(), [this](const Ball& b) { return b.GetY() > screenHeight; }), balls.end());
    if (balls.empty()) {
        lives--;
        if (lives <= 0) state = GameState::GAME_OVER;
        else balls.emplace_back(ballStart, Vector2{0, 0}, ballRadius);
    }
}

void Game::UpdatePowerUps(float dt) {
    for (auto& pu : powerUps) {
        if (!pu.active) continue;
        pu.rect.y += pu.speed;
        if (CheckCollisionRecs(pu.rect, paddle.GetRect())) {
            auto effect = PowerUpFactory::Create(pu.type, extendDuration, slowDuration, slowMultiplier);
            if (effect) {
                effect->Activate(*this);
                activeEffects.push_back(std::move(effect));
            }
            pu.active = false;
        }
        if (pu.rect.y > screenHeight) pu.active = false;
    }
    powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(), [](const PowerUp& p) { return !p.active; }), powerUps.end());
}

void Game::ResetGame() {
    score = 0; lives = 3; state = GameState::PLAYING;
    balls.clear(); balls.emplace_back(ballStart, Vector2{0, 0}, ballRadius);
    powerUps.clear(); particles.clear(); activeEffects.clear();
    extendStackCount = 0; currentSpeedMult = 1.0f;
    paddle = Paddle(screenWidth / 2 - 50, 550, 100, 20); 
    for (auto& brick : bricks) brick.SetActive(true);
}

void Game::Update() {
    if (!isNetworked) {
        if (state == GameState::PLAYING) { 
            float dt = GetFrameTime();
            HandleInput(6);
            if (IsKeyPressed(KEY_SPACE) && !balls.empty() && balls[0].GetSpeed().x == 0 && balls[0].GetSpeed().y == 0) {
                balls[0].SetSpeed(ballSpeed);
            }
            UpdateBalls();
            CheckCollisions();
            HandleBallsLost();
            UpdatePowerUps(dt);
            UpdateParticles(dt);

            for (auto& effect : activeEffects) { effect->Update(dt, *this); }
            activeEffects.erase(std::remove_if(activeEffects.begin(), activeEffects.end(), 
                [this](std::unique_ptr<PowerUpEffect>& e) { 
                    if (e->IsExpired()) { e->Deactivate(*this); return true; } return false; 
                }), activeEffects.end());

            bool allCleared = true;
            for (const auto& brick : bricks) { if (brick.IsActive()) { allCleared = false; break; } }
            if (allCleared) state = GameState::WIN;
        }
        else if ((state == GameState::GAME_OVER || state == GameState::WIN) && IsKeyPressed(KEY_SPACE)) { ResetGame(); }
        return;
    }

    // === 联机逻辑 ===
    network->Service(
        [this](const ClientInputPacket& input, ENetPeer* p) {
            opponentPaddle.SetX(input.paddleX);
        },
        [this](const ServerStatePacket& state) {
            UpdateFromNetwork(state);
        }
    );

    if (state != GameState::PLAYING) return;
    float dt = GetFrameTime();

    if (isHost) {
        // --- 主机逻辑 ---
        if (balls.empty()) balls.emplace_back(ballStart, Vector2{0, 0}, ballRadius);

        if (IsKeyDown(KEY_A)) paddle.MoveLeft(6);
        if (IsKeyDown(KEY_D)) paddle.MoveRight(6, screenWidth);

        if (IsKeyPressed(KEY_SPACE) && balls[0].GetSpeed().x == 0 && balls[0].GetSpeed().y == 0) {
            balls[0].SetSpeed(ballSpeed);
        }

        UpdateBalls();  
        CheckCollisions(); 
        
        // 主机无敌碰撞反弹对手
        for (auto& ball : balls) {
            Rectangle opRect = opponentPaddle.GetRect();
            if (CheckCollisionCircleRec(ball.GetPosition(), ballRadius, opRect)) {
                Vector2 pos = ball.GetPosition();
                Vector2 spd = ball.GetSpeed();
                if (pos.y < opRect.y + opRect.height / 2) {
                    ball.SetSpeed({ spd.x, -fabsf(spd.y) }); 
                    ball.SetPosition({ pos.x, opRect.y - ballRadius });
                } else {
                    ball.SetSpeed({ spd.x, fabsf(spd.y) });  
                    ball.SetPosition({ pos.x, opRect.y + opRect.height + ballRadius });
                }
            }
        }

        HandleBallsLost();
        UpdatePowerUps(dt);
        UpdateParticles(dt);

        for (auto& effect : activeEffects) { effect->Update(dt, *this); }
        activeEffects.erase(std::remove_if(activeEffects.begin(), activeEffects.end(), 
            [this](std::unique_ptr<PowerUpEffect>& e) { 
                if (e->IsExpired()) { e->Deactivate(*this); return true; } return false; 
            }), activeEffects.end());

        bool allCleared = true;
        for (const auto& brick : bricks) { if (brick.IsActive()) { allCleared = false; break; } }
        if (allCleared) state = GameState::WIN;

        // 【纯净版发包】不发包块ID了
        if (!balls.empty()) {
            ServerStatePacket pkt;
            pkt.ballX = balls[0].GetPosition().x;
            pkt.ballY = balls[0].GetPosition().y;
            pkt.hostPaddleX = paddle.GetRect().x;
            pkt.clientPaddleX = opponentPaddle.GetRect().x; 
            pkt.score = score;
            pkt.lives = lives;
            network->SendServerState(pkt);
        }

    } else {
        // --- 客户端逻辑 ---
        if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft(6);
        if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(6, screenWidth);

        network->SendClientInput(paddle.GetRect().x);

        // 客户端自己算碰撞，产生特效
        if (!balls.empty()) {
            CheckCollisions();
        }
        
        UpdatePowerUps(dt); 
        UpdateParticles(dt); 
    }
}

void Game::Draw() const {
    ClearBackground(BLACK);
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
    DrawText(TextFormat("Lives: %d", lives), screenWidth - 100, 10, 20, WHITE);

    if (state == GameState::PLAYING) {
        for (const auto& brick : bricks) brick.Draw();
        for (const auto& p : particles) {
            float alpha = (p.life / 0.6f) * 255.0f;
            DrawCircleV(p.position, p.size, { p.color.r, p.color.g, p.color.b, (unsigned char)alpha });
        }
        
        paddle.Draw(); 
        if (isNetworked) {
            Rectangle opRect = opponentPaddle.GetRect();
            DrawRectangleRec(opRect, WHITE); 
        }
        
        for (const auto& ball : balls) ball.Draw();
        
        for (const auto& pu : powerUps) {
            Color baseColor = (pu.type == PowerUpType::EXTEND) ? BLUE : (pu.type == PowerUpType::MULTI) ? PURPLE : ORANGE;
            DrawRectangleRec({pu.rect.x - 6, pu.rect.y - 6, pu.rect.width + 12, pu.rect.height + 12}, {baseColor.r, baseColor.g, baseColor.b, 40});
            DrawRectangleRec({pu.rect.x - 3, pu.rect.y - 3, pu.rect.width + 6, pu.rect.height + 6}, {baseColor.r, baseColor.g, baseColor.b, 80});
            DrawRectangleRec(pu.rect, baseColor);
        }
    } else if (state == GameState::GAME_OVER) {
        paddle.Draw();
        for (const auto& brick : bricks) brick.Draw();
        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});
        DrawText("GAME OVER", screenWidth / 2 - 120, screenHeight / 2, 50, RED);
        DrawText("Press SPACE", screenWidth / 2 - 100, screenHeight / 2 + 60, 30, WHITE);
    } else if (state == GameState::WIN) {
        paddle.Draw();
        for (const auto& ball : balls) ball.Draw();
        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});
        DrawText("YOU WIN!", screenWidth / 2 - 140, screenHeight / 2 - 40, 60, GREEN);
        DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 110, screenHeight / 2 + 30, 30, YELLOW);
        DrawText("Press SPACE to Restart", screenWidth / 2 - 160, screenHeight / 2 + 70, 20, WHITE);
    }
}

// 【纯净版接收】不处理碎砖ID了
void Game::UpdateFromNetwork(const ServerStatePacket& state) {
    if (balls.empty()) {
        balls.emplace_back(Vector2{state.ballX, state.ballY}, Vector2{0, 0}, ballRadius);
    }

    balls[0].SetPosition({state.ballX, state.ballY}); 

    opponentPaddle.SetX(state.hostPaddleX); 
    score = state.score;
    lives = state.lives;
}

bool Game::ShouldClose() const { return WindowShouldClose(); }
