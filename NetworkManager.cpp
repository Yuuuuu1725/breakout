#include "NetworkManager.h"
#include <iostream>
#include <cstring>

NetworkManager::NetworkManager(bool isHost) : isHost(isHost) {}

NetworkManager::~NetworkManager() {
    if (peer) enet_peer_disconnect_now(peer, 0);
    if (host) enet_host_destroy(host);
    enet_deinitialize();
}

bool NetworkManager::Init() {
    enet_initialize();
    if (isHost) {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 1234;
        host = enet_host_create(&address, 1, 2, 0, 0); // 1个客户端连接
    } else {
        host = enet_host_create(nullptr, 1, 2, 0, 0); // 客户端
    }
    return host != nullptr;
}

bool NetworkManager::Connect(const std::string& ip, int port) {
    if (isHost) return true;
    ENetAddress address;
    enet_address_set_host(&address, ip.c_str());
    address.port = port;
    peer = enet_host_connect(host, &address, 2, 0);
    return peer != nullptr;
}

void NetworkManager::Service(std::function<void(const ClientInputPacket&, ENetPeer*)> onClientInput,
                             std::function<void(const ServerStatePacket&)> onServerState) {
    if (host == nullptr) return;
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) { // 0表示非阻塞
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "A client connected!\n";
                // 【关键修复】主机在这里拿到真正的 peer 指针！
                if (isHost) peer = event.peer;
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                if (event.packet->dataLength >= sizeof(PacketType)) {
                    PacketType type = static_cast<PacketType>(*event.packet->data);
                    if (type == PACKET_CLIENT_INPUT && isHost) {
                        ClientInputPacket input;
                        memcpy(&input, event.packet->data, sizeof(ClientInputPacket));
                        onClientInput(input, event.peer);
                    } else if (type == PACKET_SERVER_STATE && !isHost) {
                        ServerStatePacket state;
                        memcpy(&state, event.packet->data, sizeof(ServerStatePacket));
                        onServerState(state);
                    }
                }
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Peer disconnected.\n";
                if (isHost) peer = nullptr; // 断开时清空指针
                event.peer->data = nullptr;
                break;
        }
    }
}

void NetworkManager::SendClientInput(float paddleX) {
    ClientInputPacket input;
    input.paddleX = paddleX;
    ENetPacket* packet = enet_packet_create(&input, sizeof(input), ENET_PACKET_FLAG_UNSEQUENCED);
    // 如果目标不存在，直接放弃发送，防止崩溃
    if (peer == nullptr) {
        enet_packet_destroy(packet);
        return;
    }
    enet_peer_send(peer, 0, packet);
}

void NetworkManager::SendServerState(const ServerStatePacket& state) {
    ENetPacket* packet = enet_packet_create(&state, sizeof(state), ENET_PACKET_FLAG_UNSEQUENCED);
    // 【致命Bug修复】必须加上这句防空指针崩溃！
    if (peer == nullptr) {
        enet_packet_destroy(packet);
        return;
    }
    enet_peer_send(peer, 0, packet);
}
