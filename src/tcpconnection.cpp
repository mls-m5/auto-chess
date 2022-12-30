/*
 * tcpconnection.cpp
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#include "factoryfunctions.h"

#include "iconnection.h"
#include "iserver.h"

#include <array>
#include <stdexcept>
#include <thread>

#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// #include <arpa/inet.h>
#include <netdb.h>

using namespace std;

namespace {

//! Converts a host name to a ip string
string getHostAddress(const string &name, short port) {
    addrinfo hints = {};

    hints.ai_family = AF_UNSPEC; // AF_INET; // only ip4
    hints.ai_socktype = SOCK_STREAM;

    addrinfo *result;
    if (getaddrinfo(name.c_str(), to_string(port).c_str(), &hints, &result)) {
        throw runtime_error("could not find address " + name);
    }

    std::shared_ptr<addrinfo> pResult(result,
                                      [](addrinfo *ptr) { freeaddrinfo(ptr); });

    for (auto p = result; p; p = p->ai_next) {
        void *address;
        string ipVersion;
        if (p->ai_family == AF_INET) {
            auto *ip4 = (sockaddr_in *)p->ai_addr;
            address = &(ip4->sin_addr);
            ipVersion = "ip4";
        }
        else {
            auto *ip6 = (sockaddr_in6 *)p->ai_addr;
            address = &(ip6->sin6_addr);
            ipVersion = "ip6";
        }

        std::array<char, INET6_ADDRSTRLEN> ipstr;
        inet_ntop(p->ai_family, address, ipstr.data(), ipstr.size());
        return ipstr.data(); // Return the first match
    }
    return "";
}

// Connect to a remote connection
class TCPConnection : public IConnection {
public:
    ~TCPConnection() {
        ::close(_socket);
    }

    TCPConnection(const std::string &hostname, short remotePort) {
        if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw std::runtime_error("could not create socket");
        }
        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_port = htons(remotePort);

        auto ipString = getHostAddress(hostname, remotePort);

        // Resolve hostname
        if (inet_pton(AF_INET, ipString.c_str(), &address.sin_addr) < 0) {
            throw std::runtime_error("could not resolve host");
        }

        if (connect(_socket,
                    reinterpret_cast<sockaddr *>(&address),
                    sizeof(address)) < 0) {
            throw std::runtime_error("could not connect to remote computer");
        }
    }

    //! Create a TCPConnection from a existing socket
    TCPConnection(int socket) {
        _socket = socket;
    }

    IConnection &sendLine(const std::string &line) override {
        if (line.back() == '\n') {
            if (send(_socket, line.c_str(), line.size(), 0) <= 0) {
                close();
            }
        }
        else {
            auto stringWithNewline = line + "\n";
            if (send(_socket,
                     stringWithNewline.c_str(),
                     stringWithNewline.size(),
                     0) <= 0) {
                close();
            }
        }
        return *this;
    }

    std::string read() override {
        std::array<char, 1024> buffer;
        auto len = ::read(_socket, buffer.data(), buffer.size());
        if (len <= 0) {
            close();
            return "";
        }
        if (buffer[len - 1] == '\n') {
            --len;
        }
        return std::string(buffer.begin(), buffer.begin() + len);
    }

    IConnection &read(std::string &line) override {
        line = read();
        if (line.empty()) {
            throw runtime_error("connection is closed");
        }
        return *this;
    }

    operator bool() override {
        return _socket > -1;
    }

private:
    void close() {
        if (_socket > 0) {
            ::close(_socket);
            _socket = -1;
        }
    }

    int _socket = -1;
};

class TCPServer : public IServer {
public:
    ~TCPServer() {
        close(_socket);
    }

    TCPServer(short port) {
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
        if (::bind(_socket,
                   reinterpret_cast<sockaddr *>(&address),
                   sizeof(address))) {
            throw runtime_error("failed to bind port " + to_string(port));
        }
    }

    // Listen and wait for clients to connect
    void listen() {
        size_t maxQueLen = 100;

        if (::listen(_socket, maxQueLen) < 0) {
            throw runtime_error("error when listening");
        }

        while (_active) {
            sockaddr_in address = {};
            socklen_t addressLength = sizeof(address);
            auto newSocket = accept(_socket,
                                    reinterpret_cast<sockaddr *>(&address),
                                    &addressLength);

            if (newSocket > -1) {
                // Add connection
                if (_callback) {
                    _callback(std::make_unique<TCPConnection>(newSocket));
                }
            }
            this_thread::sleep_for(.5s);
        }
    }

    void callback(CallbackT func) {
        _callback = func;
    }

private:
    int _socket = 0;
    int _active = true;
    CallbackT _callback;
};

} // namespace

std::unique_ptr<IServer> createTCPServer(short port) {
    return std::make_unique<TCPServer>(port);
}

std::unique_ptr<IConnection> createTCPConnection(const std::string &hostname,
                                                 short port) {
    return std::make_unique<TCPConnection>(hostname, port);
}
