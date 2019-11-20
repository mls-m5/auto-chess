
#include <iostream>
#include <vector>
#include <memory>
#include "iboard.h"

using namespace std;


int main(int argc, char **argv) {
	unique_ptr<IBoard> board(createBoard());

	board->print();

	return 0;
}

