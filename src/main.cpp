// Copyright Mattias Larsson Sköld 2019


#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#include "iboard.h"
#include "factoryfunctions.h"
#include "remoteagent.h"
#include "iconnection.h"
#include "iserver.h"

using namespace std;

int main(int argc, char **argv) {
	unique_ptr<IBoard> board(createBoard());

	// Todo: Make more generic: support tcp connection
//	unique_ptr<IConnection> connection1(createFIFOConnection("agent1r", "agent1s", true));
//	unique_ptr<IConnection> connection2(createFIFOConnection("agent2r", "agent2s", true));


	unique_ptr<IServer> server(createTCPServer(1234));

	std::vector<unique_ptr<RemoteAgent>> agents;

	server->callback([&](IConnection *connection) {
		// Remove disconnected agents
		agents.erase(remove_if(agents.begin(), agents.end(),
				[](unique_ptr<RemoteAgent> &agent) {
			return !agent->isRunning();
		}), agents.end());

		agents.emplace_back(
				new RemoteAgent(*board, move(unique_ptr<IConnection>(connection))));
	});

	board->state().print();

	agents.clear();

	server->listen();

	return 0;
}

