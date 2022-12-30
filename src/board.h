#pragma once

#include "iboard.h"
#include <mutex>
#include <string>

struct Agent {
    std::string name;
    std::string password;
    std::mutex agentMutex;
    PlayerNum playerNumber;
};

//! The board serves a game and handle players
class Board : public IBoard {
public:
    Board();

    BoardCellData operator()(int x, int y) const override;

    BoardCellData &at(int x, int y);

    BoardCellData at(int x, int y) const;

    bool move(int fromX, int fromY, int toX, int toY) override;

    BoardState state() const override;

    void wait(PlayerNum player) override;

    PlayerNum player() override;

    PlayerNum connect(std::string name, std::string password) override;

    MatchStatus matchStatus() const override;

private:
    PlayerNum otherPlayer();

    void switchPlayer();

    BoardCellData &operator()(int x, int y);

    BoardState _state;
    MatchStatus _matchStatus = MatchStatus::Normal;
    PlayerNum _currentPlayer = PlayerNum::Player1;

    std::array<Agent, 2> _agents;
    Agent &_agent1 = _agents[0];
    Agent &_agent2 = _agents[1];
};
