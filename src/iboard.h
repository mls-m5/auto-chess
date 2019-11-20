#pragma once

class IBoard {
	public:

	virtual ~IBoard() {}

	struct BoardData {
		char type = 0;
		int player = 0;
	};

	virtual void disableColors() = 0;

	virtual BoardData operator()(int x, int y) const = 0;
	virtual void print() const = 0;

	//! Move a piece and return true if successfull
	virtual bool move(int fromX, int fromY, int toX, int toY) = 0;
};


IBoard *createBoard();
