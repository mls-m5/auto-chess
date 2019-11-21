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
	return line.find(command + " ") == 0 || line == command;
}

RemoteAgent::RemoteAgent(IBoard &board, IConnection &connection):
_board(board),
_connection(connection){

	_thread = std::thread([this]() {
		startThread();
	});
}

void RemoteAgent::startThread() {
	string line;
	while (_connection.readLine(line)) {
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
				_connection.sendLine(ss.str());
			}
		}
		if (isCommand(line, "print")) {
			ostringstream ss;
			_board.print(ss);
			ss << endl << "end" << endl;
			_connection.sendLine(ss.str());
		}
		if (isCommand(line, "move")) {
			string command;
			int fromX, fromY, toX, toY;
			{
				istringstream ss(line);
				ss >> command >> fromX >> fromY >> toX >> toY;
			}
			if (_board.move(fromX, fromY, toX, toY)) {
				_connection.sendLine("ok\n");
				_board.print();
			}
			else {
				_connection.sendLine("invalid\n");
			}
		}
		if (isCommand(line, "state")) {
			auto state = _board.state();
			string stringState;
			stringState.reserve(state.size() * 2);
			for (auto cell: state) {
				stringState.push_back((cell.type == 0)? ' ': cell.type);
				stringState.push_back(cell.player + '0');
			}
			_connection.sendLine(stringState);
		}
		if (isCommand(line, "disconnect")) {
			throw runtime_error("player disconnected");
		}
	}
}

void RemoteAgent::wait() {
	_thread.join();
}



