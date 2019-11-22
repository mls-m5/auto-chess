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

	string sendFile = "agent1s";
	string receiveFile = "agent1r";
	if (argc > 1) {
		sendFile = argv[1];
	}
	if (argc > 2) {
		receiveFile = argv[2];
	}
	unique_ptr<IConnection> connection(createFIFOConnection(sendFile, receiveFile)); // Replace with more generic function
	unique_ptr<IBoard> board(connectToBoard(*connection));
	cout << "auto-chess human agent" << endl;

	board->disableColors();

	bool running = true;

	auto player = board->connect("", "");

	cout << "You are " << ((player == PlayerNum::Player1)? "white" : "black") << endl;

	board->print();

	if (player == 2) {
		cout << "waiting for other player..." << endl;
		board->wait(player);
	}

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

			while (!board->move(fromX, fromY, toX, toY)) {
				board->print();

				cout << "illegal move, try again" << endl;
			}

			cout << "ok" << endl;
			board->print();
			cout << "waiting for other player..." << endl;
			board->wait(player);
		}

		board->print();
	}
}


