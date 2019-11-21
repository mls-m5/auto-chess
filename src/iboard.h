// Copyright Mattias Larsson Sköld 2019

#pragma once

#include <array>
#include <iostream>

class IBoard {
	public:

	virtual ~IBoard() {}

	struct BoardData {
		char type = 0;
		char player = 0;
	};

	typedef std::array<BoardData, 8*8> BoardState;

	//! Reduce graphics awesomeness for terminals with less colors
	virtual void disableColors() = 0;

	//! Get a cell on a specific coordinate
	virtual BoardData operator()(int x, int y) const = 0;

	BoardData get(int x, int y) const {
		return (*this)(x, y);
	}

	//! Prints the board to a given stream
	virtual void print(std::ostream &stream = std::cout) const = 0;

	//! Move a piece and return true if successful false otherwise
	virtual bool move(int fromX, int fromY, int toX, int toY) = 0;

	// Get the whole board status
	virtual BoardState state() const = 0;
};




