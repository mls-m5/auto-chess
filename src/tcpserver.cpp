#include "tcpserver.h"
#include "tcpconnection.h"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

TCPServer::~TCPServer() {
    close(_socket);
}

TCPServer::TCPServer(short port) {
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error("could not create socket");
    }

    int option = 1;

    if (setsockopt(_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT,
                   reinterpret_cast<void *>(&option),
                   sizeof(option))) {
        throw("failed to set socket options");
    }

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind socket to port
    if (::bind(
            _socket, reinterpret_cast<sockaddr *>(&address), sizeof(address))) {
        throw std::runtime_error("failed to bind port " + std::to_string(port));
    }
}

void TCPServer::listen() {
    size_t maxQueLen = 100;

    using namespace std::chrono_literals;

    if (::listen(_socket, maxQueLen) < 0) {
        throw std::runtime_error("error when listening");
    }

    while (_active) {
        sockaddr_in address = {};
        socklen_t addressLength = sizeof(address);
        auto newSocket = accept(
            _socket, reinterpret_cast<sockaddr *>(&address), &addressLength);

        if (newSocket > -1) {
            // Add connection
            if (_callback) {
                _callback(std::make_unique<TCPConnection>(newSocket));
            }
        }
        std::this_thread::sleep_for(.5s);
    }
}
