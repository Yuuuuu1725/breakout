#include "Paddle.h"

Paddle::Paddle(float x, float y, float w, float h) {
    rect = { x, y, w, h };
    originalWidth = w;
}

void Paddle::MoveLeft(float speed) { rect.x -= speed; if(rect.x < 0) rect.x = 0; }

void Paddle::MoveRight(float speed, int screenWidth) { rect.x += speed; if(rect.x + rect.width > screenWidth) rect.x = screenWidth - rect.width; }

void Paddle::Draw() const { DrawRectangleRec(rect, BLUE); }

// 加宽：以当前中心为基准向两边延伸
void Paddle::Extend(float extraWidth) {
    float centerX = rect.x + rect.width / 2;
    rect.width += extraWidth;
    rect.x = centerX - rect.width / 2;
}

// 缩短：以当前中心为基准向两边收缩
void Paddle::Shrink(float extraWidth) {
    float centerX = rect.x + rect.width / 2;
    rect.width -= extraWidth;
    if (rect.width < originalWidth) rect.width = originalWidth; // 防止缩得比原来还短
    rect.x = centerX - rect.width / 2;
}
   
