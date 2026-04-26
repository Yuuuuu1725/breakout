#ifndef BRICK_H
#define BRICK_H
#include "raylib.h"

class Brick {
private:
    Rectangle rect;
    Color color;
    bool active;

public:
    Brick(float x, float y, float w, float h, Color c);
    void Draw() const;
    bool IsActive() const { return active; }
    void SetActive(bool a) { active = a; }
    Rectangle GetRect() const { return rect; }
    
    // 新增：获取砖块颜色，用于粒子特效
    Color GetColor() const { return color; }
};

#endif
