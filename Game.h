#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <vector>

struct Brick {
    Rectangle rect;
    bool destroyed;
    int score;
    Color color;
};

class Game {
public:
    Game();
    void Run();

private:
    void LoadLevel(int level);
    void ResetBall();
    void ResetGame();
    bool CheckWin();

    void SaveProgress();
    bool SaveExists(int lv);
    void LoadSave(int lv);

    int currentLevel;
    int score;
    int lives;

    Rectangle paddle;
    std::vector<Brick> bricks;
    float ballX, ballY;
    float ballDX, ballDY;

    float paddleSpeed;
    float ballSpeed;

    enum GameState { MENU, PLAYING, GAME_OVER };
    GameState currentState;

    const int screenWidth = 800;
    const int screenHeight = 600;
};

#endif