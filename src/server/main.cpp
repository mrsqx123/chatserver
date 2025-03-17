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

int main() {
    signal(SIGINT, &globalHandleSignal);
    signal(SIGTERM, &globalHandleSignal);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();
}