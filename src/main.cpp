// Copyright Mattias Larsson Sköld 2019


#include <iostream>
#include <vector>
#include <memory>
#include "iboard.h"
#include "factoryfunctions.h"
#include "remoteagent.h"
#include "iconnection.h"

using namespace std;

int main(int argc, char **argv) {
	unique_ptr<IBoard> board(createBoard());

	// Todo: Make more generic: support tcp connection
	unique_ptr<IConnection> connection(createFIFOConnection("player1r", "player1s", true));

	auto remoteAgent = make_unique<RemoteAgent>(*board, *connection);

	board->print();

	remoteAgent->wait();

	return 0;
}

