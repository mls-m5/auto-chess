/*
 * tcpconnection.cpp
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#include "tcpconnection.h"
#include "factoryfunctions.h"
#include "iconnection.h"
#include "iserver.h"
#include <arpa/inet.h>
#include <array>
#include <fcntl.h>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

//! Converts a host name to a ip string
std::string getHostAddress(const std::string &name, short port) {
    addrinfo hints = {};

    hints.ai_family = AF_UNSPEC; // AF_INET; // only ip4
    hints.ai_socktype = SOCK_STREAM;

    addrinfo *result;
    if (getaddrinfo(
            name.c_str(), std::to_string(port).c_str(), &hints, &result)) {
        throw std::runtime_error("could not find address " + name);
    }

    std::shared_ptr<addrinfo> pResult(result,
                                      [](addrinfo *ptr) { freeaddrinfo(ptr); });

    for (auto p = result; p; p = p->ai_next) {
        void *address;
        std::string ipVersion;
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

TCPConnection::~TCPConnection() {
    ::close(_socket);
}

TCPConnection::TCPConnection(const std::string &hostname, short remotePort) {
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

TCPConnection::TCPConnection(int socket) {
    _socket = socket;
}

IConnection &TCPConnection::sendLine(const std::string &line) {
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

std::string TCPConnection::read() {
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

IConnection &TCPConnection::read(std::string &line) {
    line = read();
    if (line.empty()) {
        throw std::runtime_error("connection is closed");
    }
    return *this;
}

TCPConnection::operator bool() {
    return _socket > -1;
}

void TCPConnection::close() {
    if (_socket > 0) {
        ::close(_socket);
        _socket = -1;
    }
}
