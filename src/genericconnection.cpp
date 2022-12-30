/*
 * genericconnection.cpp
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#include "factoryfunctions.h"
#include "iconnection.h"
#include "iserver.h"

using namespace std;

namespace {

struct Arguments {
    Arguments(int argc, char **argv) {
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            if (i + 1 < argc) {
                string value = argv[i + 1];
                // Arguments that expect a value
                if (arg == "-h" || arg == "--host") {
                    hostname = value;
                    ++i;
                }
                else if (arg == "-p" || arg == "--port") {
                    port = stoi(value);
                    ++i;
                }
                else if (arg == "-f1") {
                    sendFilename1 = value;
                    ++i;
                }
                else if (arg == "-f2") {
                    receiveFilename1 = value;
                    ++i;
                }
                else if (arg == "-f3") {
                    sendFilename2 = value;
                    ++i;
                }
                else if (arg == "-f4") {
                    receiveFilename2 = value;
                    ++i;
                }
            }
        }
    }

    enum ConnectionType { TCP, FIFO };

    ConnectionType connectionType = TCP;
    string hostname = "localhost";
    short port = 1234;

    string sendFilename1 = "agent1s";
    string receiveFilename1 = "agent1r";
    string sendFilename2 = "agent2s";
    string receiveFilename2 = "agent2r";
};

} // namespace

std::unique_ptr<IConnection> createConnection(int argc, char **argv) {
    Arguments args(argc, argv);
    switch (args.connectionType) {
    case Arguments::FIFO:
        return createFIFOConnection(args.sendFilename1, args.receiveFilename1);
    default: // TCP
        return createTCPConnection(args.hostname, args.port);
    }
}

std::unique_ptr<IServer> createServer(int argc, char **argv) {
    Arguments args(argc, argv);
    switch (args.connectionType) {
    case Arguments::FIFO:
        return createFIFOServer(args.sendFilename1,
                                args.receiveFilename1,
                                args.sendFilename2,
                                args.receiveFilename2);
    default: // TCP
        return createTCPServer(args.port);
    }
}
