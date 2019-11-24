/*
 * remoteagent.cpp
 *
 *  Created on: 21 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#include "remoteagent.h"
#include "iboard.h"
#include "iconnection.h"
#include <sstream>

using namespace std;

bool isCommand(const std::string &line, const std::string &command) {
	return line.find(command + " ") == 0 || line == command || line == (command + "\n");
}

RemoteAgent::RemoteAgent(IBoard &board, unique_ptr<IConnection> &&connection):
_board(board),
_connection(move(connection)){

	_thread = std::thread([this]() {
		try {
			startThread();
		}
		catch (runtime_error &e) {
			cout << "Agent stopped because \'" << e.what() << "\'" << endl;
		}
		catch (...) {
			cout << "Agent crashed " << endl;
		}
		_isRunning = false;
	});
}

void RemoteAgent::startThread() {
	string line;
	while (_connection->read(line)) {
		if (isCommand(line, "get")) {
			string command;
			int x, y;
			{
				istringstream ss(line);
				ss >> command >> x >> y;
			}

			auto data = _board(x, y);

			{
				ostringstream ss("cell ");
				ss << static_cast<int>(data.type) << " ";
				ss << static_cast<int>(data.player) << endl;
				_connection->sendLine(ss.str());
			}
		}
		if (isCommand(line, "print")) {
			ostringstream ss;
			_board.state().print(ss);
			ss << endl << "end" << endl;
			_connection->sendLine(ss.str());
		}
		if (isCommand(line, "move")) {
			if (!_playerNumber) {
				_connection->sendLine("denied");
			}
			if (_playerNumber != _board.player()) {
				_board.wait(_playerNumber);
			}
			string command;
			int fromX, fromY, toX, toY;
			{
				istringstream ss(line);
				ss >> command >> fromX >> fromY >> toX >> toY;
			}
			if (_board.move(fromX, fromY, toX, toY)) {
				_connection->sendLine("ok\n");
				ostringstream ss; // Buffer to avoid corrupted boards
				_board.state().print(ss);
				switch (_board.matchStatus()) {
				case MatchStatus::Chess:
					ss << "Chess" << endl;
					break;
				case MatchStatus::WhiteWon:
					ss << "White won" << endl;
					break;
				case MatchStatus::BlackWon:
					ss << "Black won" << endl;
					break;
				case MatchStatus::Draw:
					ss << "Draw" << endl;
					break;
				default:
					break;
				}
				cout << ss.str() << flush;
			}
			else {
				_connection->sendLine("invalid");
			}
		}
		if (isCommand(line, "state")) {
			_connection->sendLine(_board.state().serialize());
		}
		if (isCommand(line, "connect")) {
			istringstream ss(line);
			string command, name, password;
			ss >> command >> name >> password;
			_playerNumber = _board.connect(name, password);
			_connection->sendLine(to_string(_playerNumber));
		}
		if (isCommand(line, "disconnect")) {
			throw runtime_error("player disconnected");
		}
		if (isCommand(line, "wait")) {
			if (line == "wait") {
				_board.wait(PlayerNum::None);
				_connection->sendLine("ok\n");
			}
			else {
				istringstream ss(line);
				string command;
				int player;
				ss >> command >> player;
				_board.wait(static_cast<PlayerNum>(player));
				_connection->sendLine("ok\n");
			}
		}
		if (isCommand(line, "player")) {
			_connection->sendLine(to_string(_board.player()));
		}
		if (isCommand(line, "status")) {
			_connection->sendLine(
					to_string(static_cast<int>(_board.matchStatus())));
		}
	}
}

RemoteAgent::~RemoteAgent() {
	_connection.reset();
	wait();
}

void RemoteAgent::wait() {
	_thread.join();
}



