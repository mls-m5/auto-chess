/*
 * remoteagent.h
 *
 *  Created on: 21 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include "iboard.h"
#include <array>
#include <string>
#include <thread>

class IConnection;

//! Handles the connection to a agent in another process/on another computer
//! This is supposed to be used by the server when connecting to agents
class RemoteAgent {
public:
    RemoteAgent(IBoard &board, std::unique_ptr<IConnection> &&connection);
    ~RemoteAgent();

    void wait();

    bool isRunning() {
        return _isRunning;
    }

private:
    void startThread();
    IBoard &_board;
    std::unique_ptr<IConnection> _connection;
    std::thread _thread;

    std::string _name;
    PlayerNum _playerNumber;

    bool _isRunning = true;
};
