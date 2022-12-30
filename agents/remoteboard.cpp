// Copyright Mattias Larsson Sköld

#include "iboard.h"
#include "iconnection.h"
#include <memory>
#include <sstream>

using namespace std;

//! Handles the connection to a board an another process/computer
//! This is the interface that is supposed to be used by agents
class RemoteBoard : public IBoard {
public:
    RemoteBoard(IConnection &connection)
        : _connection(connection) {}

    ~RemoteBoard() {
        _connection.sendLine("disconnect\n");
    }

    //! Get a cell on a specific coordinate
    BoardCellData operator()(int x, int y) const override {
        ostringstream ss;
        ss << "get " << x << y << endl;
        if (!_connection.sendLine(ss.str())) {
            throw runtime_error("connection closed");
        }

        auto line = _connection.read();

        if (line.empty()) {
            throw runtime_error("connection lost to board");
        }

        istringstream received(line);

        string command;
        received >> command;

        if (command == "cell") {
            char piece;
            char player;
            received >> piece >> player;
            return {piece, static_cast<PlayerNum>(player)};
        }

        throw runtime_error("unexpected response from board");
    }

    //! Move a piece and return true if successful false otherwise
    bool move(int fromX, int fromY, int toX, int toY) override {
        ostringstream ss;
        ss << "move " << fromX << " " << fromY << " " << toX << " " << toY
           << endl;

        if (!_connection.sendLine(ss.str())) {
            throw runtime_error("connection closed");
        }

        auto line = _connection.read();
        if (line.find("ok") == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    // Get the whole board status
    BoardState state() const override {
        if (!_connection.sendLine("state")) {
            throw runtime_error("connection closed");
        }
        return BoardState(_connection.read());
    }

    void wait(PlayerNum player) override {
        ostringstream ss;
        ss << "wait " << player << endl;
        if (!_connection.sendLine(ss.str())) {
            throw runtime_error("connection closed");
        }
        _connection.read();
    }

    PlayerNum player() override {
        if (!_connection.sendLine("player")) {
            throw runtime_error("connection closed");
        }
        return static_cast<PlayerNum>(stoi(_connection.read()));
    }

    virtual PlayerNum connect(std::string name, std::string password) override {
        if (!_connection.sendLine("connect " + name + " " + password)) {
            throw runtime_error("connection closed");
        }
        return static_cast<PlayerNum>(stoi(_connection.read()));
    }

    MatchStatus matchStatus() const override {
        if (!_connection.sendLine("status")) {
            throw runtime_error("connection closed");
        }
        return static_cast<MatchStatus>(stoi(_connection.read()));
    }

private:
    IConnection &_connection;
};

std::unique_ptr<IBoard> connectToBoard(IConnection &connection) {
    return std::make_unique<RemoteBoard>(connection);
}
