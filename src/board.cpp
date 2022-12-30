// Copyright Mattias Larsson Sköld 2019

#include "board.h"

// #include "iboard.h"
// #include <iostream>
// #include <map>
// #include <memory>
// #include <string>
// #include <vector>

Board::Board() {
    _state.loadStandard();

    _agent1.playerNumber = PlayerNum::Player1;
    _agent2.playerNumber = PlayerNum::Player2;
    _agent2.agentMutex.lock();
}

BoardCellData &Board::at(int x, int y) {
    return _state(x, y);
}

BoardCellData Board::at(int x, int y) const {
    return _state(x, y);
}

bool Board::move(int fromX, int fromY, int toX, int toY) {
    if (_matchStatus >= MatchStatus::WhiteWon) {
        return false;
    }
    if (_state.move(fromX, fromY, toX, toY, player())) {
        switchPlayer();
        _matchStatus = _state.matchStatus(_currentPlayer);
        return true;
    }
    return false;
}

BoardState Board::state() const {
    return _state;
}

void Board::wait(PlayerNum player) {
    if (player == 1 || player == 2) {
        std::lock_guard lock(_agents[player - 1].agentMutex);
    }
    else {
        std::lock_guard lock(_agents[otherPlayer() - 1].agentMutex);
    }
}

PlayerNum Board::player() {
    return _currentPlayer;
}

PlayerNum Board::connect(std::string name, std::string password) {
    if (!name.empty()) {
        for (auto &agent : _agents) {
            if (agent.name == name && agent.password == password) {
                return agent.playerNumber;
            }
        }
    }

    for (auto &agent : _agents) {
        if (agent.name == "") {
            if (name.empty()) {
                agent.name = "anonymous";
            }
            else {
                agent.name = name;
            }
            return agent.playerNumber;
        }
    }

    return PlayerNum::None;
}

MatchStatus Board::matchStatus() const {
    return _matchStatus;
}

PlayerNum Board::otherPlayer() {
    return _state.otherPlayer(_currentPlayer);
}

void Board::switchPlayer() {
    _agents[_currentPlayer - 1].agentMutex.lock();
    _currentPlayer = otherPlayer();
    _agents[_currentPlayer - 1].agentMutex.unlock();
}

BoardCellData &Board::operator()(int x, int y) {
    return _state(x, y);
}

BoardCellData Board::operator()(int x, int y) const {
    return _state(x, y);
}
