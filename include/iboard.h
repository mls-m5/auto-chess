// Copyright Mattias Larsson Sköld 2019

#pragma once

#include <array>
#include <iostream>

#include "boardstate.h"


class IBoard {
	public:

	virtual ~IBoard() {}

	//! Get a cell on a specific coordinate
	virtual BoardCellData operator()(int x, int y) const = 0;

	//! Get function that is easier to use at pointers
	BoardCellData at(int x, int y) const {
		return (*this)(x, y);
	}

	//! Move a piece and return true if successful false otherwise
	//! Range for x and y is from 0 to 7
	virtual bool move(int fromX, int fromY, int toX, int toY) = 0;

	//! Wait for my turn
	//! @playerNum: The player to wait for: 0 to wait for any player
	virtual void wait(PlayerNum playerNum) = 0;

	//! The current player number
	virtual PlayerNum player() = 0;

	//! To be called as a agent before starting to play
	//! Returns the assigned number for the player
	virtual PlayerNum connect(std::string name, std::string password) = 0;

	// Get the whole board status
	virtual BoardState state() const = 0;
};




