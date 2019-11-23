/*
 * boarddata.h
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <iostream>
#include <map>


const std::map<char, std::pair<std::string, std::string> > graphics = {
	{'k', {"♔", "♚"}}, // King
	{'q', {"♕", "♛"}}, // queen
	{'r', {"♖", "♜"}}, // rook
	{'b', {"♗", "♝"}}, // bishop
	{'n', {"♘", "♞"}}, // knight
	{'p', {"♙", "♟"}}  // pawn
};

enum PlayerNum: char {
	None,
	Player1 = 1,
	Player2 = 2
};

struct BoardCellData {
	char type = 0;
	PlayerNum player = PlayerNum::None;

	bool operator ==(const BoardCellData &other) {
		return type == other.type && player == other.player;
	}

	bool operator !=(const BoardCellData &other) {
		return type != other.type || player != other.player;
	}
};


//! A single state/setup of a board
//! This is the internal state of a board and is meant to be copied around
//! BoardState does only contain information about the pieces and their position
//! but no information about the players
//! The storage size is the same size as corresponding array of BoardCellData
class BoardState: public std::array<BoardCellData, 8*8> {
public:
	BoardState() = default;
	BoardState(const BoardState&) = default;
	BoardState(const std::string stringState) {
		deserialize(stringState);
	}

	//! Serializing simply prints the board as a single line string
	//! replacing 0 with spaces
	std::string serialize() const {
		std::string stringState;
		stringState.reserve(size() * 2);
		for (auto cell: *this) {
			stringState.push_back((cell.type == 0)? ' ': cell.type);
			stringState.push_back(cell.player + '0');
		}
		return stringState;
	}

	//! the same as serialize but the other way around
	void deserialize(const std::string &stringState) {
		if (stringState.size() != size() * 2) {
			throw std::invalid_argument("serialization string is wrong length");
		}
		for (size_t i = 0; i < size(); ++i) {
			auto piece = stringState[i * 2];
			piece = (piece == ' ')? 0: piece;

			auto player = static_cast<PlayerNum>(stringState[i * 2 + 1] - '0');

			(*this)[i] = BoardCellData {piece, player};
		}
	}


	//! Empty the board
	void clear() {
		(*this) = {};
	}

	//! Load standard chess setup
	void loadStandard() {
		clear();
		setLine(0, "rnbqkbnr", Player1);
		setLine(1, "pppppppp", Player1);
		setLine(6, "pppppppp", Player2);
		setLine(7, "rnbqkbnr", Player2);
	}

	void setLine(int y, const std::string &content, PlayerNum player) {
		for (int x = 0; x < width() && x < content.size(); ++x) {
			(*this)(x, y) = {content[x], player};
		}
	}

	//! Prints the board to a given stream
	void print(std::ostream &stream = std::cout, bool enableColors = true) const {
		std::string light = "\e[48;2;50;50;50m";
		std::string dark = "\e[48;2;0;0;0m";
		if (!enableColors) {
			light = "";
			dark = "";
		}

		stream << " ";
		for (int x = 0; x < width(); ++x) {
			stream << " " << (char)('a' + x) << " ";
		}
		stream << std::endl;
		for (int y = height() - 1; y >= 0; --y) {
			stream << y + 1;
			for (int x = 0; x < width(); ++x) {
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
					try {
						auto g = (player == 2)? graphics.at(c).first: graphics.at(c).second;
						stream << " " + g + " ";
					}
					catch (...) {
						// Graphics for this piece is not defined
						stream << " ? " << std::endl;
					}
				}
			}
			if (!enableColors) {
				stream << std::endl;
			}
			else {
				stream << "\e[0m" << std::endl; // Reset colors and newline
			}
		}
		stream << std::endl;
	}


	//! Shorthand for printing without colors (BW = black and white)
	void printBW(std::ostream &stream = std::cout) {
		print(stream, false);
	}

	bool isInside(int x, int y) const {
		return x >= 0 && x < width() && y >= 0 && y <= height();
	}

	BoardCellData operator()(int x, int y) const {
		if (isInside(x, y)) {
			return (*this)[x + width() * y];
		}
		else {
			return {'x', None};
		}
	}

	BoardCellData &operator()(int x, int y) {
		if (isInside(x, y)) {
			return (*this)[x + width() * y];
		}
		else {
			throw std::out_of_range("out of range in board data");
		}
	}

	auto &at(int x, int y) {
		return (*this)(x, y);
	}

	auto at(int x, int y) const {
		return (*this)(x, y);
	}

	static int forwardDirection(PlayerNum player) {
		return (player == Player1)? 1: -1;
	}

	static PlayerNum otherPlayer(PlayerNum player) {
		return (player == PlayerNum::Player1)? PlayerNum::Player2: PlayerNum::Player1;
	}

	bool isMoveValid(int fromX, int fromY, int toX, int toY, PlayerNum player) const {
		if (!isInside(fromX, fromY)) {
			return false;
		}
		if (!isInside(toX, toY)) {
			return false;
		}

		auto fromCell = at(fromX, fromY);
		if (fromCell.player != player) {
			// A player cannot move the other players pieces
			return false;
		}

		if (fromX == toY && fromY == toY) {
			return false;
		}

		auto toCell = at(toX, toY);

		if (toCell.player == player) {
			// A player cannot attack its own pieces
			return false;
		}

		if (fromCell.type == 'p') { // Pawn
			if (toCell.player == otherPlayer(player)) {
				if (abs(fromX - toX) == 1 && toX - fromX == forwardDirection(player)) {
					return true;
				}
			}
			else {
				if (fromX == toX && (toY - fromY) * forwardDirection(player) > 0) {
					if (abs(toY - fromY) == 1) {
						return true;
					}
					else if (abs(toY - fromY) == 2) {
						if (at(fromX, fromY + forwardDirection(player)).type == 0) {
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
			if (abs(toX - fromX) <= 1 && abs(toY - fromY) <= 1) {
				return true;
			}
		}
		else if (fromCell.type == 'r') {
			if (toX == fromX || toY == fromY) {
				return isPathClear(fromX, fromY, toX, toY, player);
			}
		}
		else if (fromCell.type == 'b') {
			if (abs(toX - fromX) == abs(toY - fromY)) {
				return isPathClear(fromX, fromY, toX, toY, player);
			}
		}
		else if (fromCell.type == 'q') {
			return isPathClear(fromX, fromY, toX, toY, player);
		}

		return false;
	}


	//! Checks lines and diagonals if it is okay to move
	bool isPathClear(int fromX, int fromY, int toX, int toY, PlayerNum player) const {
		auto dx = fromX - toX;
		auto dy = fromY - toY;
		if (dx == 0) {
			// Vertical movement
			auto length = abs(dy);
			auto direction = dy / length;
			for (int i = 0; i < length; ++i) {
				auto ty = fromY + i * direction;
				if (at(fromX, ty).type != 0) {
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
				if (at(tx, fromY).type != 0) {
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
				if (at(tx, ty).type != 0) {
					return false;
				}
			}
		}
		else {
			return false; // The path is not straight
		}
		auto targetCell = at(toX, toY);

		if (targetCell.type != 0 && targetCell.player == player) {
			return false;
		}
		return true;
	}

	//! Move a piece and return true if successful
	bool move(int fromX, int fromY, int toX, int toY, PlayerNum player) {
		if (isMoveValid(fromX, fromY, toX, toY, player)) {
			(*this)(toX, toY) = {};
			std::swap((*this)(toX, toY), (*this)(fromX, fromY));

			return true;
		}
		else {
			return false;
		}
	}


	static constexpr size_t width() {
		return 8;
	}

	static constexpr size_t height() {
		return 8;
	}

	bool operator == (const BoardState &other) {
		for (int i = 0; i < size(); ++i) {
			if ((*this)[i] != other[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator != (const BoardState &other) {
		for (int i = 0; i < size(); ++i) {
			if ((*this)[i] != other[i]) {
				return true;
			}
		}
		return false;
	}
};


inline std::ostream& operator << (std::ostream &stream, const BoardState &state) {
	state.print(stream, true);
	return stream;
}
