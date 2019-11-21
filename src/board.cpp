
#include "iboard.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace std;

map<char, std::pair<string, string> > graphics = {
	{'k', {"♔", "♚"}}, // King
	{'q', {"♕", "♛"}}, // queen
	{'r', {"♖", "♜"}}, // rook
	{'b', {"♗", "♝"}}, // bishop
	{'n', {"♘", "♞"}}, // knight
	{'p', {"♙", "♟"}}  // pawn
};


class Board: public IBoard {
public:

	Board() {
		for (int i = 0; i < _width; ++i) {
			_data[i + _width * 1] = {'p', 1};
			_data[i + _width * (_height - 2)] = {'p', 2};
		}

		setLine(0, "rnbqkbnr", 1);
		setLine(7, "rnbqkbnr", 2);
	}

	BoardData operator()(int x, int y) const override {
		return _data[x + _width * y];
	}

	auto &get(int x, int y) {
		return (*this)(x, y);
	}

	auto get(int x, int y) const {
		return (*this)(x, y);
	}

	bool isInside(int x, int y) {
		return x >= 0 && x < _width && y >= 0 && y <= _height;
	}

	static int forwardDirection(int player) {
		return (player == 1)? 1: -1;
	}

	int forwardDirection() {
		return forwardDirection(_currentPlayer);
	}

	//! Checksi lines and diagonals if it is okay to move
	bool isPathClear(int fromX, int fromY, int toX, int toY) {
		auto dx = fromX - toX;
		auto dy = fromY - toY;
		if (dx == 0) {
			// Vertical movement
			auto length = abs(dy);
			auto direction = dy / length;
			for (int i = 0; i < length; ++i) {
				auto ty = fromY + i * direction;
				if (get(fromX, ty).type != 0) {
					return false;
				}
			}
		}
		else if (dy == 0) {
			// Horizontal movement
			auto length = abs(dx);
			auto direction = dx / length;
			for (int i = 0; i < length; ++i) {
				auto tx = fromX + i * direction;
				if (get(tx, fromY).type != 0) {
					return false;
				}
			}
		}
		else if (abs(dx) == abs(dy)) {
			auto length = abs(dx); // Note that |dx| = |dy|
			auto dirx = dx / length;
			auto diry = dy / length;

			for (int i = 0; i < length; ++i) {
				auto tx = fromX + i * dirx;
				auto ty = fromY + i * diry;
				if (get(tx, ty).type != 0) {
					return false;
				}
			}
		}
		else {
			return false; // The path is not straight
		}
		auto targetCell = get(toX, toY);

		if (targetCell.type != 0 && targetCell.player == _currentPlayer) {
			return false;
		}
		return true;
	}

	bool isMoveValid(int fromX, int fromY, int toX, int toY) {
		if (!isInside(fromX, fromY)) {
			return false;
		}
		if (!isInside(toX, toY)) {
			return false;
		}
		
		auto fromCell = get(fromX, fromY);
		if (fromCell.player != _currentPlayer) {
			// A player cannot move the other players pieces
			return false;
		}

		if (fromX == toY && fromY == toY) {
			return false;
		}

		auto toCell = get(toX, toY);

		if (toCell.player == _currentPlayer) {
			// A player cannot attack its own pieces
			return false;
		}

		if (fromCell.type == 'p') { // Pawn
			if (toCell.player == otherPlayer()) {
				if (abs(fromX - toX) == 1 && toX - fromX == forwardDirection()) {
					return true;
				}
			}
			else {
				if (fromX == toX && (toY - fromY) * forwardDirection() > 0) {
					if (abs(toY - fromY) == 1) {
						return true;
					}
					else if (abs(toY - fromY) == 2) {
						if (get(fromX, fromY + forwardDirection()).type == 0) {
							return true;
						}
					}
				}
			}
		}
		else if (fromCell.type == 'n') {
			if (abs(toX - fromX) == 2 && abs(toY - fromY) == 1) {
				return true;
			}
			else if (abs(toX - fromX) == 1 && abs(toY - fromY) == 2) {
				return true;
			}
		}
		else if (fromCell.type == 'k') {
			if (abs(toX - fromX) <= 1 && abs(toY - toY) <= 1) {
				return true;
			}
		}
		else if (fromCell.type == 'r') {
			if (toX == fromX || toY == fromY) {
				return isPathClear(fromX, fromY, toX, toY);
			}
		}
		else if (fromCell.type == 'b') {
			if (abs(toX - fromX) == abs(toY - fromY)) {
				return isPathClear(fromX, fromY, toX, toY);
			}
		}
		else if (fromCell.type == 'q') {
			return isPathClear(fromX, fromY, toX, toY);
		}

		return false;
	}

	//! Move a piece and return true if successful
	bool move(int fromX, int fromY, int toX, int toY) override {
		if (isMoveValid(fromX, fromY, toX, toY)) {
			(*this)(toX, toY) = {};
			swap((*this)(toX, toY), (*this)(fromX, fromY));

			switchPlayer();
			return true;
		}
		else {
			return false;
		}
	}

	void print(std::ostream &stream = cout) const override{
		string light = "\e[48;2;50;50;50m";
		string dark = "\e[48;2;0;0;0m";
		if (_disableColors) {
			light = "";
			dark = "";
		}
		stream << " ";
		for (int x = 0; x < _width; ++x) {
			stream << " " << (char)('a' + x) << " ";
		}
		stream << endl;
		for (int y = _height - 1; y >= 0; --y) {
			stream << y + 1;
			for (int x = 0; x < _width; ++x) {
				if ((y % 2) != (x % 2)) {
					stream << dark;
				}
				else {
					stream << light;
				}
				auto c = (*this)(x, y).type;
				auto player = (*this)(x, y).player;
				if (c == 0) {
					stream << "   ";
				}
				else {
					auto g = (player == 2)? graphics[c].first: graphics[c].second;
					stream << " "s + g + " "s;
				}
			}
			if (_disableColors) {
				stream << endl;
			}
			else {
				stream << "\e[0m" << endl; // Reset colors and newline
			}
		}
		stream << endl;
	}

	void disableColors() override {
		_disableColors = true;
	}

	const BoardState state() override {
		return _data;
	}

private:

	int otherPlayer() {
		return (_currentPlayer == 1)? 2: 1;
	}
	void switchPlayer() {
		_currentPlayer = otherPlayer();
	}

	BoardData &operator()(int x, int y) {
		return _data[x + _width * y];
	}

	void setLine(int y, string content, char player) {
		for (int x = 0; x < _width && x < content.size(); ++x) {
			(*this)(x, y) = {content[x], player};
		}
	}

	array <BoardData, 8*8> _data;
	int _width = 8;
	int _height = 8;
	int _currentPlayer = 1;

	bool _disableColors = false;
};

IBoard *createBoard() {
    return new Board;
}
