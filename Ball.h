#ifndef BALL_H
#define BALL_H
#include "raylib.h"

class Ball {
private:
    Vector2 position;
    Vector2 speed;
    float radius;
    float speedMultiplier; // 新增：速度倍率，用于减速道具

public:
    Ball(Vector2 pos, Vector2 sp, float r);
    void Move();
    void Draw() const;
    void BounceEdge(int screenWidth, int screenHeight);
    void BouncePaddle(Rectangle paddleRect);
    bool CollideWithBrick(Rectangle brickRect);
    void Reset(Vector2 pos, Vector2 sp);
    
    float GetY() const { return position.y; }
    Vector2 GetPosition() const { return position; }
    Vector2 GetSpeed() const { return speed; }
    void SetPosition(Vector2 pos) { position = pos; }
    void SetSpeed(Vector2 sp) { speed = sp; }

    // 新增：控制速度倍率
    void SetSpeedMultiplier(float mult) { speedMultiplier = mult; }
};

#endif
