// Copyright Mattias Larsson Sköld

#include "iboard.h"
#include "iconnection.h"
#include <sstream>

using namespace std;

class RemoteBoard: public IBoard {
public:
	RemoteBoard(IConnection &connection):
	_connection(connection){
	}

	~RemoteBoard() {
		_connection.sendLine("disconnect\n");
	}

	//! Reduce graphics awesomeness for terminals with less colors
	void disableColors() override {
		_connection.sendLine("colors disable");
	}

	//! Get a cell on a specific coordinate
	BoardData operator()(int x, int y) const override {
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
			return {piece, player};
		}

		// Todo: Handle this better
		throw runtime_error("unexpected command from board");
	}

	//! Prints the board to a given stream
	void print(std::ostream &stream = std::cout) const override {
		_connection.sendLine("print\n");

		string line;

		while (_connection.readLine(line)) {
			if (line.find("end") == 0) {
				break;
			}
			stream << line << endl;
		}
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

		auto response = _connection.readLine();
		BoardState state;
		for (size_t i = 0;
				i < state.size() * 2 && i < response.size();
				i += 2) {
			auto piece = response[i];
			auto player = response[i + 1] - '0';
			piece = (piece == ' ')? 0: piece;
			state[i] = BoardData {piece, player};
		}
		return state;
	}

private:
	IConnection &_connection;
};


IBoard *connectToBoard(IConnection &connection) {
	return new RemoteBoard(connection);
}
