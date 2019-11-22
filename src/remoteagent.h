/*
 * remoteagent.h
 *
 *  Created on: 21 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <thread>
#include <string>
#include <array>

class IBoard;
class IConnection;

class RemoteAgent {
public:
	RemoteAgent(IBoard &board, IConnection &connection);

	void wait();
private:
	void startThread();
	IBoard &_board;
	IConnection &_connection;
	std::thread _thread;

	std::string name;
	char playerNumber;
};



