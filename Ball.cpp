#include "Ball.h"
#include "CollisionUtils.h" 
#include <cmath>

Ball::Ball(Vector2 pos, Vector2 sp, float r) {
    position = pos;
    speed = sp;
    radius = r;
    speedMultiplier = 1.0f; // 默认正常速度
}

void Ball::Move() {
    // 移动时乘以倍率
    position.x += speed.x * speedMultiplier;
    position.y += speed.y * speedMultiplier;
}

void Ball::Draw() const {
    DrawCircleV(position, radius, RED);
}

void Ball::BounceEdge(int screenWidth, int screenHeight) {
    if (position.x - radius <= 0 || position.x + radius >= screenWidth) speed.x *= -1;
    if (position.y - radius <= 0) speed.y *= -1;
}

void Ball::BouncePaddle(Rectangle paddleRect) {
    if (speed.y > 0 && CheckCollisionCircleRec(position, radius, paddleRect)) {
        speed.y *= -1;
        position.y = paddleRect.y - radius;
    }
}

bool Ball::CollideWithBrick(Rectangle brickRect) {
    CCircle circle = { position.x, position.y, radius };
    CRect rect = { brickRect.x, brickRect.y, brickRect.width, brickRect.height };
    if (CheckCollision(circle, rect)) {
        bool isHorizontal = ResolveCollision(circle, rect);
        if (isHorizontal) speed.x *= -1;
        else speed.y *= -1;
        position.x = circle.x;
        position.y = circle.y;
        return true;
    }
    return false;
}

void Ball::Reset(Vector2 pos, Vector2 sp) {
    position = pos;
    speed = sp;
    speedMultiplier = 1.0f; // 重置时恢复速度
}
   
