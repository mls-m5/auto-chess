#pragma once
#include "iserver.h"

class TCPServer : public IServer {
public:
    ~TCPServer();

    TCPServer(short port);

    // Listen and wait for clients to connect
    void listen();

    void callback(CallbackT func) {
        _callback = func;
    }

private:
    int _socket = 0;
    int _active = true;
    CallbackT _callback;
};
