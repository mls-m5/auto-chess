/*
 * boarddata.h
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>


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


enum class MatchStatus {
	Normal,
	Chess,
	WhiteWon,
	BlackWon,
	Draw,
};


struct BoardCellData {
	char type = 0;
	PlayerNum player = PlayerNum::None;

	bool operator ==(const BoardCellData &other) const {
		return type == other.type && player == other.player;
	}

	bool operator !=(const BoardCellData &other) const {
		return type != other.type || player != other.player;
	}
};



struct BoardTransition {
	BoardTransition() {* this = {};};
	BoardTransition(int fromX, int fromY, int toX, int toY):
		fromX(fromX), fromY(fromY), toX(toX), toY(toY) {}

	char fromX, fromY;
	char toX, toY;
};


//! A single state/setup of a board
//! This is the internal state of a board and is meant to be copied around
//! BoardState does only contain information about the pieces and their position
//! but no information about the players
//! The storage size is the same size as corresponding array of BoardCellData
//! plus keeping track of king and rook movement to verify if castling is allowed
class BoardState: public std::array<BoardCellData, 8*8> {
private:
	std::array<unsigned char, 2> _castlingStatus = {};

	static constexpr unsigned KingMoved =       0b0001;
	static constexpr unsigned LeftRookMoved =   0b0010;
	static constexpr unsigned RightRookMoved =  0b0100;

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
		return x >= 0 && x < width() && y >= 0 && y < height();
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

	const auto at(int x, int y) const {
		return (*this)(x, y);
	}

	static int forwardDirection(PlayerNum player) {
		return (player == Player1)? 1: -1;
	}

	static PlayerNum otherPlayer(PlayerNum player) {
		return (player == PlayerNum::Player1)? PlayerNum::Player2: PlayerNum::Player1;
	}

	//! Check if the king is threatened
	//! Also known as "chess"
	bool isChess(PlayerNum player) {
		// Find the king
		int kingX, kingY;
		for (int y = 0; y < height(); ++y) {
			for (int x = 0; x < width(); ++x) {
				auto &cell = at(x, y);
				if (cell.type == 'k' && cell.player == player) {
					kingX = x;
					kingY = y;
					break;
				}
			}
		}

		auto theOtherPlayer = otherPlayer(player);

		// Can any of the other players pieces attack the king
		for (int y = 0; y < height(); ++y) {
			for (int x = 0; x < width(); ++x) {
				auto &cell = at(x, y);
				if (cell.player == theOtherPlayer) {
					if (canPieceMoveTo(x, y, kingX, kingY, theOtherPlayer)) {
						return true;
					}
				}
			}
		}
		return false;
	}

	//! Returns the status of the game before a player is going to make a move
	//! @player the player that is just about to make a move
	MatchStatus matchStatus(PlayerNum player) {
		auto moves = getValidMoves(player);
		if (moves.empty()) {
			if (isChess(player)) {
				if (player == PlayerNum::Player1) {
					return MatchStatus::BlackWon;
				}
				else {
					return MatchStatus::WhiteWon;
				}
			}
			else {
				return MatchStatus::Draw;
			}
		}
		return MatchStatus::Normal;
	}


	bool isMatchFinished(PlayerNum player) {
		return matchStatus(player) >= MatchStatus::WhiteWon;
	}


	//! Determine if making a move will put the player in chess
	//! and will therefore be illegal
	bool willMoveThreatOwnKing(
			BoardTransition transition, PlayerNum player) const {
		BoardState newState = *this;
		newState.moveUnchecked(transition);
		return newState.isChess(player);
	}

	//! The last check before making a move
	//! If this returns true it is perfectly okay to move the piece in a game
	bool isMoveValid(int fromX, int fromY, int toX, int toY, PlayerNum player) const {
		return isMoveValid({fromX, fromY, toX, toY}, player);
	}

	bool isMoveValid(BoardTransition t, PlayerNum player) const {
		if (isCastling(t)) {
			return isCastlingValid(t);
		}
		else {
			return canPieceMoveTo(t.fromX, t.fromY, t.toX, t.toY, player) &&
					!willMoveThreatOwnKing(t, player);
		}
	}

	//! Check if a piece can move to a position
	//! Do not check for chess
	bool canPieceMoveTo(int fromX, int fromY, int toX, int toY, PlayerNum player) const {
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
				if (abs(fromX - toX) == 1 && (toY - fromY) == forwardDirection(player)) {
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
		auto dx = toX - fromX;
		auto dy = toY - fromY;
		if (dx == 0) {
			// Vertical movement
			auto length = abs(dy);
			auto direction = dy / length;
			for (int i = 1; i < length; ++i) {
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
			for (int i = 1; i < length; ++i) {
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

			for (int i = 1; i < length; ++i) {
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


	//! Get moves that is allowed without ending up in chess
	std::vector<BoardTransition> getValidMoves() {
		return {};
	}


	//! Get all possible movements for a single piece
	std::vector<BoardTransition> getPossibleMoves(int x, int y) {
		std::vector<BoardTransition> ret;
		auto &cell = at(x, y);
		auto type = cell.type;
		auto player = cell.player;
		auto theOtherPlayer = otherPlayer(player);

		auto addMove = [&](int toX, int toY) {
			ret.insert(ret.end(), {x, y, toX, toY});
		};

		auto tryMove = [&](int toX, int toY) {
			if (canPieceMoveTo(x, y, toX, toY, player)) {
				addMove(toX, toY);
				return true;
			}
			else {
				return false;
			}
		};

		auto tryDirection = [&](int dx, int dy) {
			for (int i = 1; i < 8; ++i) {
				auto tx = x + dx * i;
				auto ty = y + dy * i;
				if (tx < 0 || tx >= width() || ty < 0 || ty >= height()) {
					return;
				}
				auto &cell = at(tx, ty);
				auto &cellPlayer = cell.player;
				if (cellPlayer == theOtherPlayer) {
					addMove(tx, ty);
					return;
				}
				else if (cell.player == player) {
					return;
				}
				else {
					addMove(tx, ty);
				}
			}
		};

		auto tryStraights = [&]() {
			tryDirection(-1, 0);
			tryDirection(+1, 0);
			tryDirection(0, +1);
			tryDirection(0, -1);
		};

		auto tryDiagonals = [&]() {
			tryDirection(-1, -1);
			tryDirection(+1, -1);
			tryDirection(+1, +1);
			tryDirection(+1, -1);
		};

		switch (type) {
			case 'p':
			{
				auto forward = forwardDirection(player);
				if (tryMove(x, y + forward)) {
					tryMove(x, y + forward * 2);
				}
				tryMove(x + 1, y + forward);
				tryMove(x - 1, y + forward);
				break;
			}

			case 'k':
				tryMove(x + 1, y);
				tryMove(x + 1, y + 1);
				tryMove(x, y + 1);
				tryMove(x - 1, y + 1);
				tryMove(x - 1, y);
				tryMove(x - 1, y - 1);
				tryMove(x, y - 1);
				tryMove(x - 1, y - 1);
				break;

			case 'n':
				tryMove(x + 2, y + 1);
				tryMove(x + 1, y + 2);
				tryMove(x + 2, y - 1);
				tryMove(x + 1, y - 2);
				tryMove(x - 2, y + 1);
				tryMove(x - 1, y + 2);
				tryMove(x - 2, y - 1);
				tryMove(x - 1, y - 2);
				break;

			case 'r':
				tryStraights();
				break;

			case 'b':
				tryDiagonals();
				break;

			case 'q':
				tryStraights();
				tryDiagonals();
				break;

			default:
				break;
		}

		return ret;
	}

	//! Get moves that is allowed by piece restrictions
	//! Note that this does not check if the move leads to chess or not
	//! For that, use getValidMoves
	std::vector<BoardTransition> getPossibleMoves(PlayerNum player) {
		std::vector<BoardTransition> ret;

		for (int y = 0; y < height(); ++y) {
			for (int x = 0; x < width(); ++x) {
				if (at(x, y).player == player) {
					auto moves = getPossibleMoves(x, y);
					ret.insert(ret.end(), moves.begin(), moves.end());
				}
			}
		}

		return ret;
	}

	//! Get a list of all possible moves for player
	std::vector<BoardTransition> getValidMoves(PlayerNum player, bool includeCastling = false) {
		auto ret = getPossibleMoves(player);
		auto removeFrom = std::remove_if(
				ret.begin(),
				ret.end(),
				[this, player](BoardTransition& transition) {
			auto newState = *this;
			newState.moveUnchecked(transition);
			// Remove all moves that leads to chess
			return newState.isChess(player);
		});
		ret.erase(removeFrom, ret.end());

		if (includeCastling) {
			auto y = (player == PlayerNum::Player1)? 0: 7;
			for (BoardTransition transition: {
				BoardTransition(4, y, 2, y), BoardTransition(4, y, 4, 6)}) {
				if (isCastlingValid(transition)) {
					ret.push_back(transition);
				}
			}
		}
		return ret;
	}

	//! Recognize if the current movement is a castling move
	bool isCastling(BoardTransition transition) const {
		auto &fromCell = at(transition.fromX, transition.fromY);
		auto player = (transition.fromY == 0)? PlayerNum::Player1: PlayerNum::Player2;
		if (fromCell.type == 'k') {
			if (transition.toX > transition.fromX) {
				if (at(7, transition.toY).type != 'r') {
					return false;
				}
				if (castlingStatus(player) & (KingMoved | RightRookMoved)) {
					return false;
				}
			}
			else {
				if (at(0, transition.fromY).type != 'r') {
					return false;
				}
				if (castlingStatus(player) & (KingMoved | LeftRookMoved)) {
					return false;
				}
			}
			if (transition.fromX == 4 && (transition.fromY == 0 || transition.fromY == 7)) {
				if (transition.fromY == transition.toY) {
					return true;
				}
			}
		}
		return false;
	}

	//! Try to do castling without doing any checks whatsoever
	//! note that fromX is assumed to be 0 or 7
	void doCastlingUnchecked(BoardTransition transition) {
		auto y = transition.fromY;

		PlayerNum player = (transition.fromY == 0)? PlayerNum::Player1: PlayerNum::Player2;
		castlingStatus(player) |= 0b0111; // All future castling disabled

		std::swap(at(transition.fromX, y), at(transition.toX, y));
		if (transition.toX == 6) {
			std::swap(at(7, y), at(5, y));

		}
		else {
			std::swap(at(0, y), at(3, y));
		}
	}


	unsigned char castlingStatus(PlayerNum player) const {
		return _castlingStatus[(player == PlayerNum::Player1)? 0: 1];
	}

	unsigned char &castlingStatus(PlayerNum player) {
		if (player == Player1) {
			return _castlingStatus[0];
		}
		else {
			return _castlingStatus[1];
		}
	}


	void kingMoved(PlayerNum player) {
		castlingStatus(player) |= KingMoved;
	}

	void leftRookMoved(PlayerNum player) {
		castlingStatus(player) |= LeftRookMoved;
	}

	void rightRookMoved(PlayerNum player) {
		castlingStatus(player) |= RightRookMoved;
	}

	bool isKingMoved(PlayerNum player) const {
		return castlingStatus(player) & KingMoved;
	}

	bool isLeftRookMoved(PlayerNum player) const {
		return castlingStatus(player) & LeftRookMoved;
	}

	bool isRightRookMoved(PlayerNum player) const {
		return castlingStatus(player) & RightRookMoved;
	}

	bool isCastlingValid(BoardTransition transition) const {
		if (!isCastling(transition)) {
			return false;
		}

		PlayerNum player = (transition.fromY == 0)? PlayerNum::Player1: PlayerNum::Player2;

		if (isKingMoved(player)) {
			return false;
		}

		auto y = transition.fromY;

		// Check if
		// 1. The path is free
		// 2. Moving the king does not lead to chess at any point during the move
		if (transition.toX > transition.fromX) {
			if (isRightRookMoved(player)) {
				return false;
			}
			for (int x = 5; x < 7; ++x) {
				if (at(x, y).player) {
					return false;
				}
				auto state = *this;
				state.moveUnchecked({4, y, x, y});
				if (state.isChess(player)) {
					return false;
				}
			}
		}
		else {
			if (isLeftRookMoved(player)) {
				return false;
			}
			for (int x = 1; x < 4; ++x) {
				if (at(x, y).player) {
					return false;
				}
			}

			for (int x = 2; x < 4; ++x) {
				auto state = *this;
				state.moveUnchecked({4, y, x, y});
				if (state.isChess(player)) {
					return false;
				}
			}
		}

		return true;
	}

	//! If the king or some rook is moved this will affect if castling is allowed
	//! this function keeps track of what is allowed after a move
	void markCastlingChange(int x, int y) {
		auto player = PlayerNum::None;

		if (y == 0) {
			player = PlayerNum::Player1;
		}
		else if (y == 7) {
			player = PlayerNum::Player2;
		}
		else {
			return;
		}

		if (x == 0) {
			castlingStatus(player) |= LeftRookMoved;
		}
		else if (x == 7) {
			castlingStatus(player) |= RightRookMoved;
		}
		else if (x == 4) {
			castlingStatus(player) |= KingMoved;
		}
	}

	//! Make changes to state without checking if the move is valid
	void moveUnchecked(BoardTransition transition) {
		// Check if castling
		auto &fromCell = at(transition.fromX, transition.fromY);
		auto &toCell = at(transition.toX, transition.toY);

		if (isCastling(transition)) {
			doCastlingUnchecked(transition);
		}
		else {
			toCell = {};
			std::swap(toCell, fromCell);

			markCastlingChange(transition.fromX, transition.fromY);
		}
	}

	//! Move a piece and return true if successful
	bool move(char fromX, char fromY, char toX, char toY, PlayerNum player) {
		if (isMoveValid(fromX, fromY, toX, toY, player)) {
			moveUnchecked({fromX, fromY, toX, toY});

			return true;
		}
		else {
			return false;
		}
	}

	//! same as other move but using BoardTransition that is easier to store
	bool move(BoardTransition transition, PlayerNum player) {
		return move(
				transition.fromX,
				transition.fromY,
				transition.toX,
				transition.toY,
				player);
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
