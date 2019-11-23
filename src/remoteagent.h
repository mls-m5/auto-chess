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
#include "iboard.h"

class IConnection;

//! Handles the connection to a agent in another process/on another computer
//! This is supposed to be used by the server when connecting to agents
class RemoteAgent {
public:
	RemoteAgent(IBoard &board, IConnection &connection);

	void wait();
private:
	void startThread();
	IBoard &_board;
	IConnection &_connection;
	std::thread _thread;

	std::string _name;
	PlayerNum _playerNumber;
};



