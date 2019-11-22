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
	unique_ptr<IConnection> connection1(createFIFOConnection("agent1r", "agent1s", true));
	unique_ptr<IConnection> connection2(createFIFOConnection("agent2r", "agent2s", true));

	auto remoteAgent1 = make_unique<RemoteAgent>(*board, *connection1);
	auto remoteAgent2 = make_unique<RemoteAgent>(*board, *connection2);

	board->print();

	remoteAgent1->wait();
	remoteAgent2->wait();

	return 0;
}

