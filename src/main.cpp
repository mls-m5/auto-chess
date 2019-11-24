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
	unique_ptr<IServer> server(createServer(argc, argv));

	std::vector<unique_ptr<RemoteAgent>> agents;

	auto removeDisconnectedAgents = [&]() {
		agents.erase(remove_if(agents.begin(), agents.end(),
				[](unique_ptr<RemoteAgent> &agent) {
			return !agent->isRunning();
		}), agents.end());
	};

	server->callback([&](IConnection *connection) {
		removeDisconnectedAgents();

		agents.emplace_back(
				new RemoteAgent(*board, move(unique_ptr<IConnection>(connection))));
	});

	board->state().print();

	agents.clear();

	server->listen();

	return 0;
}

