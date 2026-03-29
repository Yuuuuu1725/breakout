#include "Ball.h"

Ball::Ball(float x, float y, float radius, float speedX, float speedY) {
    this->x = x;
    this->y = y;
    this->radius = radius;
    this->speedX = speedX;
    this->speedY = speedY;
}

void Ball::Update() {
    x += speedX;
    y += speedY;
}

void Ball::Draw() {
    DrawCircle(x, y, radius, RED);
}

void Ball::Reset() {
    x = 400;
    y = 300;
    speedX = 3;
    speedY = 3;
}