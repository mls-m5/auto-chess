// Copyright Mattias Larsson Sköld 2019

#include "iboard.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <mutex>

using namespace std;


struct Agent {
	string name;
	string password;
	mutex agentMutex;
	PlayerNum playerNumber;
};


//! The board serves a game and handle players
class Board: public IBoard {
public:

	Board() {
		_state.loadStandard();

		_agent1.playerNumber = PlayerNum::Player1;
		_agent2.playerNumber = PlayerNum::Player2;
		_agent2.agentMutex.lock();
	}

	BoardCellData operator()(int x, int y) const override {
		return _state(x, y);
	}

	auto &at(int x, int y) {
		return _state(x, y);
	}

	auto at(int x, int y) const {
		return _state(x, y);
	}

	//! Checks lines and diagonals if it is okay to move
	bool isPathClear(int fromX, int fromY, int toX, int toY) const {
		return _state.isPathClear(fromX, fromY, toX, toY, _currentPlayer);
	}

	bool isMoveValid(int fromX, int fromY, int toX, int toY) {
		return _state.isMoveValid(fromX, fromY, toX, toY, player());
	}

	bool move(int fromX, int fromY, int toX, int toY) override {
		if (_state.move(fromX, fromY, toX, toY, player())) {
			switchPlayer();
			return true;
		}
		return false;
	}

	BoardState state() const override {
		return _state;
	}

	void wait(PlayerNum player) override {
		if (player == 1 || player == 2) {
			auto lock = lock_guard(_agents[player - 1].agentMutex);
		}
		else {
			auto lock = lock_guard(_agents[otherPlayer() - 1].agentMutex);
		}
	}

	PlayerNum player() override {
		return _currentPlayer;
	}

	PlayerNum connect(std::string name, std::string password) override {
		if (!name.empty()) {
			for (auto &agent: _agents) {
				if (agent.name == name && agent.password == password) {
					return agent.playerNumber;
				}
			}
		}

		for (auto &agent: _agents) {
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

	static constexpr size_t width() {
		return BoardState::width();
	}

	static constexpr size_t height() {
		return BoardState::height();
	}

private:


	PlayerNum otherPlayer() {
		return _state.otherPlayer(_currentPlayer);
	}

	void switchPlayer() {
		_agents[_currentPlayer - 1].agentMutex.lock();
		_currentPlayer = otherPlayer();
		_agents[_currentPlayer - 1].agentMutex.unlock();
	}

	BoardCellData &operator()(int x, int y) {
		return _state(x, y);
	}


	BoardState _state;
	PlayerNum _currentPlayer = PlayerNum::Player1;

	array <Agent, 2> _agents;
	Agent & _agent1 = _agents[0];
	Agent & _agent2 = _agents[1];
};

IBoard *createBoard() {
    return new Board;
}
