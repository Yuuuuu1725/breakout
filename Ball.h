#ifndef BALL_H
#define BALL_H

#include "raylib.h"

class Ball {
public:
    float x, y;
    float radius;
    float speedX, speedY;

    Ball(float x, float y, float radius, float speedX, float speedY);
    void Update();
    void Draw();
    void Reset();
};

#endif