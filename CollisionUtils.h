#ifndef COLLISION_UTILS_H
#define COLLISION_UTILS_H
#include <cmath>

// 轻量级结构体，不依赖 Raylib
struct CRect { float x, y, width, height; };
struct CCircle { float x, y, radius; };

inline bool CheckCollision(CCircle circle, CRect rect) {
    float testX = circle.x;
    float testY = circle.y;
    if (circle.x < rect.x) testX = rect.x;
    else if (circle.x > rect.x + rect.width) testX = rect.x + rect.width;
    if (circle.y < rect.y) testY = rect.y;
    else if (circle.y > rect.y + rect.height) testY = rect.y + rect.height;
    float distX = circle.x - testX;
    float distY = circle.y - testY;
    float distance = sqrt((distX*distX) + (distY*distY));
    return distance <= circle.radius;
}

// 返回 true 表示发生水平碰撞，false 表示垂直碰撞
inline bool ResolveCollision(CCircle& circle, CRect rect) {
    float dx = circle.x - (rect.x + rect.width / 2.0f);
    float dy = circle.y - (rect.y + rect.height / 2.0f);
    float overlapX = (rect.width / 2.0f + circle.radius) - fabs(dx);
    float overlapY = (rect.height / 2.0f + circle.radius) - fabs(dy);
    if (overlapX < overlapY) {
        circle.x += (dx > 0 ? overlapX : -overlapX);
        return true; // 水平碰撞
    } else {
        circle.y += (dy > 0 ? overlapY : -overlapY);
        return false; // 垂直碰撞
    }
}
#endif
