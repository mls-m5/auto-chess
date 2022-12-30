// Copyright Mattias Larsson Sköld

#include "remoteboard.h"
#include "iboard.h"
#include "iconnection.h"
#include <memory>
#include <sstream>

RemoteBoard::RemoteBoard(IConnection &connection)
    : _connection(connection) {}

RemoteBoard::~RemoteBoard() {
    _connection.sendLine("disconnect\n");
}

BoardCellData RemoteBoard::operator()(int x, int y) const {
    std::ostringstream ss;
    ss << "get " << x << y << std::endl;
    if (!_connection.sendLine(ss.str())) {
        throw std::runtime_error("connection closed");
    }

    auto line = _connection.read();

    if (line.empty()) {
        throw std::runtime_error("connection lost to board");
    }

    std::istringstream received(line);

    std::string command;
    received >> command;

    if (command == "cell") {
        char piece;
        char player;
        received >> piece >> player;
        return {piece, static_cast<PlayerNum>(player)};
    }

    throw std::runtime_error("unexpected response from board");
}

bool RemoteBoard::move(int fromX, int fromY, int toX, int toY) {
    std::ostringstream ss;
    ss << "move " << fromX << " " << fromY << " " << toX << " " << toY
       << std::endl;

    if (!_connection.sendLine(ss.str())) {
        throw std::runtime_error("connection closed");
    }

    auto line = _connection.read();
    if (line.find("ok") == 0) {
        return true;
    }
    else {
        return false;
    }
}

BoardState RemoteBoard::state() const {
    if (!_connection.sendLine("state")) {
        throw std::runtime_error("connection closed");
    }
    return BoardState(_connection.read());
}

void RemoteBoard::wait(PlayerNum player) {
    std::ostringstream ss;
    ss << "wait " << player << std::endl;
    if (!_connection.sendLine(ss.str())) {
        throw std::runtime_error("connection closed");
    }
    _connection.read();
}

PlayerNum RemoteBoard::player() {
    if (!_connection.sendLine("player")) {
        throw std::runtime_error("connection closed");
    }
    return static_cast<PlayerNum>(stoi(_connection.read()));
}

PlayerNum RemoteBoard::connect(std::string name, std::string password) {
    if (!_connection.sendLine("connect " + name + " " + password)) {
        throw std::runtime_error("connection closed");
    }
    return static_cast<PlayerNum>(stoi(_connection.read()));
}

MatchStatus RemoteBoard::matchStatus() const {
    if (!_connection.sendLine("status")) {
        throw std::runtime_error("connection closed");
    }
    return static_cast<MatchStatus>(stoi(_connection.read()));
}
