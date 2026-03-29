#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"

class Brick {
public:
    float x, y;
    float width, height;
    bool destroyed;

    Brick();
    Brick(float x, float y, float w, float h);
    void Draw();
};

#endif