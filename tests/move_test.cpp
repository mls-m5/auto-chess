// Copyright Mattias Larsson Sköld 2019


#include "mls-unit-test/unittest.h"
#include "iboard.h"
#include <memory>
#include "factoryfunctions.h"

using namespace std;

TEST_SUIT_BEGIN


TEST_CASE("move pawn") {
	unique_ptr<IBoard> board(createBoard());
	board->state().printBW();

	ASSERT(board->move(2, 1, 2, 3), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(3, 6, 3, 4), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(2, 3, 3, 4), "Could not move pawn");
	board->state().printBW();
}


TEST_CASE("move pawn, black attack") {
	unique_ptr<IBoard> board(createBoard());

	board->state().printBW();

	ASSERT(board->move(3, 1, 3, 3), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(4, 6, 4, 4), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(7, 1, 7, 3), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(4, 4, 3, 3), "Could not move pawn");
	board->state().printBW();
}

TEST_CASE("stop invalid moves") {
	unique_ptr<IBoard> board(createBoard());

	ASSERT_EQ(false, board->move(4, 1, 5, 6)) // Crazy pawn jump
	ASSERT_EQ(false, board->move(4, 6, 4, 5)) // Wrong player
}



TEST_CASE("move knight") {
	unique_ptr<IBoard> board(createBoard());
	ASSERT_EQ(false, board->move(1, 0, 1, 2));
	ASSERT(board->move(1, 0, 2, 2), "Could not move knight");

	board->state().printBW();
}

TEST_CASE("Should not move king to chess") {
	BoardState state;
	state(1, 7) = {'k', PlayerNum::Player1};
	state(0, 0) = {'r', PlayerNum::Player2};

	// King will be threatened by rook
	ASSERT_EQ(false, state.move(1, 7, 0, 7, PlayerNum::Player1))
}

TEST_CASE("move rook") {
	BoardState state;
	state(0, 0) = {'r', PlayerNum::Player1};
	ASSERT_EQ(false, state.move(0, 0, 1, 7, PlayerNum::Player1));
	ASSERT_EQ(true, state.move(0, 0, 0, 7, PlayerNum::Player1));
}

TEST_CASE("move queen") {
	BoardState state;
	state(0, 0) = {'q', PlayerNum::Player1};
	ASSERT_EQ(false, state.move(0, 0, 6, 7, PlayerNum::Player1));
	ASSERT_EQ(true, state.move(0, 0, 7, 7, PlayerNum::Player1));
}


TEST_CASE("move bishop") {
	BoardState state;
	state(0, 0) = {'b', PlayerNum::Player1};
	ASSERT_EQ(false, state.move(0, 0, 6, 7, PlayerNum::Player1));
	ASSERT_EQ(true, state.move(0, 0, 7, 7, PlayerNum::Player1));
}

TEST_CASE("is chess?") {
	BoardState state;
	state(0, 0) = {'k', PlayerNum::Player1};
	state(0, 1) = {'r', PlayerNum::Player2};
	ASSERT(state.isChess(PlayerNum::Player1), "could not recognize chess");
}

TEST_CASE("is mate?") {
	unique_ptr<IBoard> board(createBoard());
	// Fools mate
	board->move(5, 1, 5, 2);
	ASSERT_EQ(false, board->state().isMatchFinished(board->player()));
	board->move(4, 6, 4, 4);
	ASSERT_EQ(false, board->state().isMatchFinished(board->player()));
	board->move(6, 1, 6, 3);
	ASSERT_EQ(false, board->state().isMatchFinished(board->player()));
	board->move(3, 7, 7, 3);

	auto state = board->state();

	state.printBW();

	auto moves = state.getValidMoves(PlayerNum::Player1);

	for (auto move: moves) {
		BoardState possibleState = state;
		if (!possibleState.move(move, PlayerNum::Player1)) {
			cout << "This is proposed as a possible move" << endl;
			cout << "but it wasnt" << endl;
			possibleState.moveUnchecked(move);
		}

		possibleState.printBW();
	}

	ASSERT(board->matchStatus() == MatchStatus::BlackWon,
			"could not recognize mate");
}

TEST_CASE("get all valid moves") {
	unique_ptr<IBoard> board(createBoard());
	auto state = board->state();
	auto moves = state.getValidMoves(PlayerNum::Player1);
	ASSERT_EQ(moves.size(), 8 * 2 + 2 * 2);
}

TEST_CASE("get all possible moves") {
	unique_ptr<IBoard> board(createBoard());
	auto state = board->state();
	auto moves = state.getPossibleMoves(PlayerNum::Player1);
	ASSERT_EQ(moves.size(), 8 * 2 + 2 * 2);
}

TEST_CASE("do castling") {
	const BoardCellData whiteKing = {'k', PlayerNum::Player1};

	{
		BoardState state;
		state(4, 0) = whiteKing;
		state(0, 0) = {'r', PlayerNum::Player1};

		ASSERT_EQ(true, state.move(4, 0, 2, 0, PlayerNum::Player1));

		state.printBW();

		ASSERT(whiteKing == state(2, 0), "move check failed");
	}
	{
		BoardState state;
		BoardCellData whiteKing = {'k', PlayerNum::Player1};
		state(4, 0) = whiteKing;
		state(7, 0) = {'r', PlayerNum::Player1};

		ASSERT_EQ(true, state.move(4, 0, 6, 0, PlayerNum::Player1));

		state.printBW();

		ASSERT(whiteKing == state(6, 0), "move check failed");
	}
}

TEST_CASE("prevent castling from moved pieces") {
	{
		BoardState state;
		BoardCellData whiteKing = {'k', PlayerNum::Player1};
		state(4, 0) = whiteKing;
		state(7, 0) = state(0, 0) = {'r', PlayerNum::Player1};
		state.printBW();

		ASSERT_EQ(true, state.move(7, 0, 7, 6, PlayerNum::Player1));
		ASSERT_EQ(true, state.move(7, 6, 7, 0, PlayerNum::Player1));
		state.printBW();
		ASSERT_EQ(false, state.move(4, 0, 6, 0, PlayerNum::Player1));
		ASSERT_EQ(true, state.move(4, 0, 2, 0, PlayerNum::Player1));

		state.printBW();
	}
}

TEST_SUIT_END
