#pragma once

#include "iboard.h"
#include "iconnection.h"

//! Handles the connection to a board an another process/computer
//! This is the interface that is supposed to be used by agents
class RemoteBoard : public IBoard {
public:
    RemoteBoard(IConnection &connection);

    ~RemoteBoard();

    //! Get a cell on a specific coordinate
    BoardCellData operator()(int x, int y) const override;

    //! Move a piece and return true if successful false otherwise
    bool move(int fromX, int fromY, int toX, int toY) override;

    // Get the whole board status
    BoardState state() const override;

    void wait(PlayerNum player) override;

    PlayerNum player() override;

    PlayerNum connect(std::string name, std::string password) override;

    MatchStatus matchStatus() const override;

private:
    IConnection &_connection;
};
