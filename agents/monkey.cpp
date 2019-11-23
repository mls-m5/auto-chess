// Copyright Mattias Larsson Sköld 2019

#include "iboard.h"
#include "iconnection.h"
#include "factoryfunctions.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <random>

using namespace std;

int main(int argc, char ** argv) {
	cout << "This is the monkey AI!" << endl;

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

//	board->disableColors();

	bool running = true;

	auto player = board->connect("monkey", "");

	cout << "You are " << ((player == PlayerNum::Player1)? "white" : "black") << endl;

	board->state().print();

	if (player == 2) {
		cout << "waiting for other player..." << endl;
		board->wait(player);
	}

	random_device device;
	mt19937 generator(device());
	uniform_int_distribution<std::mt19937::result_type> distribution(0,7);

	size_t tries = 0;
	// Super advanced AI stuff
	while (true) {
		auto fromX = distribution(generator);
		auto fromY = distribution(generator);
		auto toX = distribution(generator);
		auto toY = distribution(generator);
		++ tries;
		if (board->move(fromX, fromY, toX, toY)) {
			board->state().print();

			cout << "move successful after only " << tries << " tries" << endl;
			tries = 0;
		}
	}

	return 0;
}


