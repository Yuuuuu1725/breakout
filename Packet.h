#ifndef PACKET_H
#define PACKET_H
#include <cstdint>

enum PacketType : uint8_t {
    PACKET_CLIENT_INPUT = 1,
    PACKET_SERVER_STATE = 2
};

// 客户端发给服务器：自己的挡板X坐标
struct ClientInputPacket {
    PacketType type = PACKET_CLIENT_INPUT;
    float paddleX;
};

// 服务器发给客户端：游戏核心状态
struct ServerStatePacket {
    PacketType type = PACKET_SERVER_STATE;
    float ballX, ballY;
    float hostPaddleX, clientPaddleX; // 两块板的位置
    int score;
    int lives;
    // 如果要做完整的砖块同步，可以加一个 bool bricksActive[100]; 
    // 这里为了符合“至少球同步”的基础要求，先省略，主机的砖块消除逻辑照常运行
};

#endif
