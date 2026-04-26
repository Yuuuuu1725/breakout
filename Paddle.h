#ifndef PADDLE_H
#define PADDLE_H
#include "raylib.h"

class Paddle {
private:
    Rectangle rect;
    float originalWidth; // 新增：记录原始宽度

public:
    Paddle(float x, float y, float w, float h);
    void MoveLeft(float speed);
    void MoveRight(float speed, int screenWidth = 800);
    void Draw() const;
    Rectangle GetRect() const { return rect; }
    
    // 新增：加宽和缩短（保持中心点不动）
    void Extend(float extraWidth);
    void Shrink(float extraWidth);
    void SetX(float x) { rect.x = x; }
   
};
   
#endif
