#include "Brick.h"

// 修改：接收颜色参数并赋值
Brick::Brick(float x, float y, float w, float h, Color c) {
    rect = {x, y, w, h};
    active = true;
    color = c;
}

void Brick::Draw() const {
    // 修改：使用对象自己的 color 而不是硬编码的 GREEN
    if(active) DrawRectangleRec(rect, color); 
}
