/*
 * boardstate_test.cpp
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#include "mls-unit-test/unittest.h"

#include "boardstate.h"

using namespace std;


TEST_SUIT_BEGIN

TEST_CASE("is initialized with zeros") {
	BoardState state;

	for (auto cell: state) {
		ASSERT_EQ(cell.type, 0);
		ASSERT_EQ(cell.player, 0);
	}
}

TEST_CASE("no distortion when sending state") {
	BoardState state1;
	BoardState state2;

	ASSERT(state1 == state2, "states is not equal in beginning");

	state1.loadStandard();

	ASSERT(state1 != state2, "states should not be equal");

	state2.deserialize(state1.serialize());

	cout << "state 1: " << endl;
	state1.printBW();

	cout << "state 2: " << endl;
	state2.printBW();

	ASSERT(state1 == state2, "serialization transfer failed");
}

TEST_SUIT_END




