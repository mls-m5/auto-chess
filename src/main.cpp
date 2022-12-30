// Copyright Mattias Larsson Sköld 2019

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "factoryfunctions.h"
#include "iboard.h"
#include "iconnection.h"
#include "iserver.h"
#include "remoteagent.h"

int main(int argc, char **argv) {
    auto board = createBoard();
    auto server = createServer(argc, argv);

    std::vector<std::unique_ptr<RemoteAgent>> agents;

    auto removeDisconnectedAgents = [&]() {
        agents.erase(remove_if(agents.begin(),
                               agents.end(),
                               [](std::unique_ptr<RemoteAgent> &agent) {
                                   return !agent->isRunning();
                               }),
                     agents.end());
    };

    server->callback([&](std::unique_ptr<IConnection> connection) {
        removeDisconnectedAgents();

        agents.emplace_back(
            std::make_unique<RemoteAgent>(*board, std::move(connection)));
    });

    board->state().print();

    agents.clear();

    server->listen();

    return 0;
}
