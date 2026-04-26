#include "Game.h"
#include "NetworkManager.h"
#include <iostream>
#include <unistd.h> // Linux下的延时库

int main(int argc, char* argv[]) {
    // 默认单机
    bool isNetworked = false;
    bool isHost = false;
    std::string ip = "127.0.0.1";

    // 解析参数： ./BrickBreaker server 或 ./BrickBreaker client 192.168.1.100
    if (argc > 1) {
        isNetworked = true;
        std::string mode = argv[1];
        if (mode == "server" || mode == "host") {
            isHost = true;
            std::cout << "Starting as HOST...\n";
        } else if (mode == "client") {
            isHost = false;
            if (argc > 2) ip = argv[2];
            std::cout << "Starting as CLIENT connecting to " << ip << "...\n";
        }
    }

    InitWindow(1050, 600, isHost ? "Brick Breaker (HOST)" : (isNetworked ? "Brick Breaker (CLIENT)" : "Brick Breaker"));
    SetTargetFPS(60);

    Game game("config.json");

    NetworkManager* network = nullptr;
    if (isNetworked) {
        network = new NetworkManager(isHost);
        if (network->Init()) {
            if (!isHost) {
                network->Connect(ip);
                // 等待连接成功 (简单处理，正式项目需要状态机)
                usleep(500000); 
            }
            game.SetNetworkMode(network, isHost);
        } else {
            std::cerr << "Failed to initialize network!\n";
            return -1;
        }
    }

    while (!game.ShouldClose()) {
        game.Update();
        BeginDrawing();
        game.Draw();
        EndDrawing();
    }

    if (network) delete network;
    CloseWindow();
    return 0;
}
