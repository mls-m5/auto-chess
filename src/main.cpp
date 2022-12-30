// Copyright Mattias Larsson Sköld 2019

#include "board.h"
#include "factoryfunctions.h"
#include "iconnection.h"
#include "iserver.h"
#include "remoteagent.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char **argv) {
    auto board = Board{};
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
            std::make_unique<RemoteAgent>(board, std::move(connection)));
    });

    board.state().print();

    agents.clear();

    server->listen();

    return 0;
}
