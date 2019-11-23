// Copyright Mattias Larsson Sköld

#include "iboard.h"
#include "iconnection.h"
#include <sstream>

using namespace std;

//! Handles the connection to a board an another process/computer
//! This is the interface that is supposed to be used by agents
class RemoteBoard: public IBoard {
public:
	RemoteBoard(IConnection &connection):
	_connection(connection){
	}

	~RemoteBoard() {
		_connection.sendLine("disconnect\n");
	}

	//! Get a cell on a specific coordinate
	BoardCellData operator()(int x, int y) const override {
		ostringstream ss;
		ss << "get " << x << y << endl;
		_connection.sendLine(ss.str());

		auto line = _connection.readLine();

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
		ss << "move " << fromX << " " << fromY << " " << toX << " " << toY << endl;

		_connection.sendLine(ss.str());

		auto line = _connection.readLine();
		if (line.find("ok") == 0) {
			return true;
		}
		else {
			return false;
		}
	}

	// Get the whole board status
	BoardState state() const override {
		_connection.sendLine("state");
		return BoardState(_connection.readLine());
	}

	void wait(PlayerNum player) override {
		ostringstream ss;
		ss << "wait " << player << endl;
		_connection.sendLine(ss.str());
		_connection.readLine();
	}

	PlayerNum player() override {
		_connection.sendLine("player");
		return static_cast<PlayerNum>(stoi(_connection.readLine()));
	}

	virtual PlayerNum connect(std::string name, std::string password) override {
		_connection.sendLine("connect " + name + " " + password);
		return static_cast<PlayerNum>(stoi(_connection.readLine()));
	}

private:
	IConnection &_connection;
};


IBoard *connectToBoard(IConnection &connection) {
	return new RemoteBoard(connection);
}
