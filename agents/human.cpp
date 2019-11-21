// Copyright Mattias Larsson Sköld 2019

#include "iboard.h"
#include "iconnection.h"
#include "factoryfunctions.h"

#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

void printInvalidInfo() {
	cout << "invalid move" << endl;
	cout << "format: [from] [to]" << endl;
	cout << "eg: a1 a3" << endl;
}

int main(int argc, char ** argv) {
	cout << "Welcome human!" << endl;

//	unique_ptr<IConnection> connection(createConnection(argc, argv));
	unique_ptr<IConnection> connection(createFIFOConnection("player1s", "player1r")); // Replace with more generic function
	unique_ptr<IBoard> board(connectToBoard(*connection));
	cout << "auto-chess human agent" << endl;

//	board->disableColors();

	bool running = true;

	board->print();

	string line;
	while (getline(cin, line)) {
		if (line[0] >= 'a' && line[0] <= 'h') {
			istringstream ss(line);
			string from;
			string to;
			if (ss >> from) {
				ss >> to;
			}
			else {
				printInvalidInfo();
				continue;
			}

			if (from.size() != 2 || to.size() != 2) {
				printInvalidInfo();
				continue;
			}

			int fromX = from[0] - 'a';
			int fromY = from[1] - '1';

			int toX = to[0] - 'a';
			int toY = to[1] - '1';

			if (!board->move(fromX, fromY, toX, toY)) {
				cout << "illegal move" << endl;
			}
		}

		board->print();
	}
}


