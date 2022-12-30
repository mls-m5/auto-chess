/*
 * factoryfunctions.h
 *
 *  Created on: 21 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <memory>
#include <string>

//! Connection suitable based on command line arguments
std::unique_ptr<class IConnection> createConnection(int argc, char **argv);

//! Create a generic server
//! Different type of server will be generated depending on arguments
std::unique_ptr<class IServer> createServer(int argc, char **argv);

std::unique_ptr<class IConnection> createTCPConnection(
    const std::string &hostname, short port);

std::unique_ptr<class IServer> createTCPServer(short port);

//! Create a named pipe connection
//! if create is true new fifo files is created
//! Can only be used on the same machine
std::unique_ptr<class IConnection> createFIFOConnection(
    const std::string &sendFilename,
    const std::string &receiveFilename,
    bool create = false);

std::unique_ptr<class IServer> createFIFOServer(const std::string &agent1s,
                                                const std::string &agent1r,
                                                const std::string &agent2s,
                                                const std::string &agent2r);

//! create a local board
std::unique_ptr<class IBoard> createBoard();

//! create a connection to a remote board
std::unique_ptr<class IBoard> connectToBoard(class IConnection &connection);
