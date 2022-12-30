/*
 * factoryfunctions.h
 *
 *  Created on: 21 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <string>

//! Connection suitable based on command line arguments
class IConnection *createConnection(int argc, char **argv);

//! Create a generic server
//! Different type of server will be generated depending on arguments
class IServer *createServer(int argc, char **argv);

class IConnection *createTCPConnection(const std::string &hostname, short port);

class IServer *createTCPServer(short port);

//! Create a named pipe connection
//! if create is true new fifo files is created
//! Can only be used on the same machine
class IConnection *createFIFOConnection(const std::string &sendFilename,
                                        const std::string &receiveFilename,
                                        bool create = false);

class IServer *createFIFOServer(const std::string &agent1s,
                                const std::string &agent1r,
                                const std::string &agent2s,
                                const std::string &agent2r);

//! create a local board
class IBoard *createBoard();

//! create a connection to a remote board
class IBoard *connectToBoard(class IConnection &connection);
