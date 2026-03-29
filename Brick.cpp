#include "Brick.h"

Brick::Brick() {
    x = y = width = height = 0;
    destroyed = false;
}

Brick::Brick(float x, float y, float w, float h) {
    this->x = x;
    this->y = y;
    width = w;
    height = h;
    destroyed = false;
}

void Brick::Draw() {
    DrawRectangle((int)x, (int)y, (int)width, (int)height, GREEN);
}