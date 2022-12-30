#pragma once

#include "iconnection.h"

// Connect to a remote connection
class TCPConnection : public IConnection {
public:
    ~TCPConnection();

    TCPConnection(const std::string &hostname, short remotePort);

    //! Create a TCPConnection from a existing socket
    TCPConnection(int socket);

    IConnection &sendLine(const std::string &line) override;

    std::string read() override;

    IConnection &read(std::string &line) override;

    operator bool() override;

private:
    void close();

    int _socket = -1;
};
