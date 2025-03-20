#include <iostream>
#include <csignal>
#include "chatserver.hpp"
#include "chatservice.hpp"

using namespace std;

void globalHandleSignal(int sig) {
    cout << "Caught signal: " << sig << endl;
    ChatService::instance()->handleSignal(sig);
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, &globalHandleSignal);
    signal(SIGTERM, &globalHandleSignal);

    if (argc < 3) {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = stoi(argv[2]);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}