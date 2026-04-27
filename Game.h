#pragma once
#include "raylib.h"
#include <vector>
#include <memory>

class Game;
enum GameState { MENU, PLAYING, GAME_OVER };

enum class PowerUpType {
    PADDLE_EXTEND,
    MULTI_BALL,
    SLOW_BALL
};

struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float life;
};

class PowerUpEffect {
public:
    virtual void Apply(Game& game) = 0;
    virtual ~PowerUpEffect() = default;
};

class ExtendPaddleEffect : public PowerUpEffect {
public:
    void Apply(Game& game) override;
};

class MultiBallEffect : public PowerUpEffect {
public:
    void Apply(Game& game) override;
};

class SlowBallEffect : public PowerUpEffect {
public:
    void Apply(Game& game) override;
};

std::unique_ptr<PowerUpEffect> CreatePowerUp(PowerUpType type);

class Game {
public:
    const int screenWidth = 800;
    const int screenHeight = 600;

    struct Ball {
        Vector2 pos;
        Vector2 speed;
        float radius;
        bool active;
    };

    std::vector<Ball> balls;
    Rectangle paddle;
    float paddleSpeed;
    float originalPaddleWidth;
    std::vector<std::vector<bool>> destroyed;
    int score;
    int lives;
    GameState currentState;
    int currentLevel;
    int brickRows;
    bool infiniteLives;
    Rectangle checkBoxRect;
    Rectangle backButton;

    struct PowerUpItem {
        Vector2 pos;
        PowerUpType type;
        float speed;
    };

    std::vector<PowerUpItem> powerUps;
    std::vector<Particle> particles;
    const float POWERUP_DURATION = 10.0f;

    bool hasPaddleExtend;
    bool hasMultiBall;
    bool hasSlowBall;
    float paddleExtendTimer;
    float multiBallTimer;
    float slowBallTimer;
    bool isSlowed;

    bool isLoading;
    float loadTimer;
    float showLoadedTimer;

public:
    Game();
    void Run();
    void ResetGame();
    void ResetBall();
    bool CheckAllBricksDestroyed();
    void SpawnParticles(Vector2 pos, Color color);
    void SpawnPowerUp(Vector2 pos);
    void ExtendPaddle(float ratio);
    void RestorePaddleSize();
    void AddMultiBall();
    void SlowDownBalls();
    void RestoreBallSpeed();
    void GenerateRandomBricks();
};
