// Copyright Mattias Larsson Sköld 2019


#include "mls-unit-test/unittest.h"
#include "iboard.h"
#include <memory>
#include "factoryfunctions.h"

using namespace std;

TEST_SUIT_BEGIN


TEST_CASE("Move pawn") {
	unique_ptr<IBoard> board(createBoard());
	board->state().printBW();

	ASSERT(board->move(2, 1, 2, 3), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(3, 6, 3, 4), "Could not move pawn");
	board->state().printBW();
	ASSERT(board->move(2, 3, 3, 4), "Could not move pawn");
	board->state().printBW();
}


TEST_CASE("Move pawn, black attack") {
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



TEST_CASE("Move knight") {
	unique_ptr<IBoard> board(createBoard());
	ASSERT(board->move(1, 0, 2, 2), "Could not move knight");

	board->state().printBW();

}

TEST_SUIT_END
