#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "PowerUpEffects.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <memory>
#include "NetworkManager.h" 

enum class GameState { PLAYING, GAME_OVER, WIN };

struct PowerUp {
    Rectangle rect;
    PowerUpType type;
    float speed;
    bool active;
};

struct Particle {
    Vector2 position;
    Vector2 speed;
    Color color;
    float life;
    float size;
};

class Game {
private:
    int screenWidth, screenHeight, score, lives;
    GameState state;

    std::vector<Ball> balls;
    Vector2 ballStart;
    Vector2 ballSpeed;
    int ballRadius;

    Paddle paddle;
    std::vector<Brick> bricks;
    int scorePerBrick;

    std::vector<PowerUp> powerUps;
    std::vector<Particle> particles;
    std::vector<std::unique_ptr<PowerUpEffect>> activeEffects;

    float powerUpDropChance;
    float extendDuration;
    float slowDuration;
    float slowMultiplier;

    void InitBricks(int rows, int cols, float startX, float startY, float spacingX, float spacingY, float w, float h, const nlohmann::json& colors);
    void HandleInput(int paddleSpeed);
    void UpdateBalls();
    void CheckCollisions();
    void HandleBallsLost();
    void UpdatePowerUps(float dt);
    void SpawnParticles(Vector2 pos, Color color, int count);
    void UpdateParticles(float dt);
    void ResetGame();
    Paddle opponentPaddle; // 对手挡板
    NetworkManager* network;
    bool isNetworked = false;
    bool isHost = false;
    Vector2 prevBallPos, targetBallPos;
    float interpolationTimer = 0.0f;
    const float INTERP_DELAY = 0.05f;
public:
    Game(const std::string& configPath);
    void Update();
    void Draw() const;
    bool ShouldClose() const;

    int extendStackCount = 0;
    float currentSpeedMult = 1.0f;
    
    void ExtendPaddle(float amount);
    void ShrinkPaddle(float amount);
    void SetBallSpeedMult(float mult);
    void ResetBallSpeedMult();
    void SpawnExtraBall();
    void SetNetworkMode(NetworkManager* net, bool host) { network = net; isNetworked = true; isHost = host; }
    void UpdateFromNetwork(const ServerStatePacket& state); // 客户端调用
};

#endif
