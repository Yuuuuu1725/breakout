#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include <string>
#include <functional>
#include <enet/enet.h>
#include "Packet.h"

class NetworkManager {
private:
    ENetHost* host = nullptr;
    ENetPeer* peer = nullptr; // 对于Server来说，这是连上来的Client；对于Client来说，这是Server
    bool isHost;

public:
    NetworkManager(bool isHost);
    ~NetworkManager();

    bool Init(); // 创建Host
    bool Connect(const std::string& ip = "127.0.0.1", int port = 1234); // Client调用
    void Service(std::function<void(const ClientInputPacket&, ENetPeer*)> onClientInput,
                 std::function<void(const ServerStatePacket&)> onServerState);

    void SendClientInput(float paddleX);
    void SendServerState(const ServerStatePacket& state);
};

#endif
