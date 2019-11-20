
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
		_data.resize(_width * _height);
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

		return false;
	}

	//! Move a piece and return true if successfull
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

	void print() const override{
		string light = "\e[48;2;50;50;50m";
		string dark = "\e[48;2;0;0;0m";
		if (_disableColors) {
			light = "";
			dark = "";
		}
		cout << " ";
		for (int x = 0; x < _width; ++x) {
			cout << " " << (char)('a' + x) << " ";
		}
		cout << endl;
		for (int y = _height - 1; y >= 0; --y) {
			cout << y + 1;
			for (int x = 0; x < _width; ++x) {
				if ((y % 2) != (x % 2)) {
					cout << dark;
				}
				else {
					cout << light;
				}
				auto c = (*this)(x, y).type;
				auto player = (*this)(x, y).player;
				if (c == 0) {
					cout << "   ";
				}
				else {
					auto g = (player == 2)? graphics[c].first: graphics[c].second;
					cout << " "s + g + " "s;
				}
			}
			if (_disableColors) {
				cout << endl;
			}
			else {
				cout << "\e[0m" << endl; // Reset colors and newline
			}
		}
		cout << endl;
	}

	void disableColors() override {
		_disableColors = true;
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

	void setLine(int y, string content, int player) {
		for (int x = 0; x < _width && x < content.size(); ++x) {
			(*this)(x, y) = {content[x], player};
		}
	}

	vector <BoardData> _data;
	int _width = 8;
	int _height = 8;
	int _currentPlayer = 1;

	bool _disableColors = false;
};

IBoard *createBoard() {
    return new Board;
}
