#pragma once
#include "raylib.h"
#include <vector>

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    const int screenWidth = 800;
    const int screenHeight = 600;

    Vector2 ballPos;
    Vector2 ballSpeed;
    float radius;

    Rectangle paddle;
    float paddleSpeed;

    std::vector<std::vector<bool>> destroyed;
    int score;
    int lives;
    GameState currentState;

    int currentLevel;
    int brickRows;
    std::vector<int> brickScores;

    bool infiniteLives = false;
    Rectangle checkBoxRect = { 250, 450, 30, 30 };
    Rectangle backButton = { 650, 500, 120, 40 }; // 这里！

    void ResetGame();
    bool CheckAllBricksDestroyed();

public:
    Game();
    void Run();
};