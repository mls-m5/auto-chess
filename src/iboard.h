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

	//! Prints the board to a given stream
	virtual void print(std::ostream &stream = std::cout) const = 0;

	//! Move a piece and return true if successfull false otherwise
	virtual bool move(int fromX, int fromY, int toX, int toY) = 0;

	// Get the whole board status
	virtual const BoardState state() = 0;
};


IBoard *createBoard();
