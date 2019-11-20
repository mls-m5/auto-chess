
#include "mls-unit-test/unittest.h"
#include "iboard.h"
#include <memory>

using namespace std;

TEST_SUIT_BEGIN


TEST_CASE("Move pawn") {
	unique_ptr<IBoard> board(createBoard());
	board->disableColors();

	board->print();

	ASSERT(board->move(2, 1, 2, 3), "Could not move pawn");
	board->print();
	ASSERT(board->move(3, 6, 3, 4), "Could not move pawn");
	board->print();
	ASSERT(board->move(2, 3, 3, 4), "Could not move pawn");
	board->print();
}


TEST_CASE("Move knight") {
	unique_ptr<IBoard> board(createBoard());
	board->disableColors();

	ASSERT(board->move(1, 0, 2, 2), "Could not move knight");

	board->print();

}

TEST_SUIT_END
