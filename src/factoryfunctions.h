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

//! Create a file stream connection
//! if create is true new fifo files is created
class IConnection *createFIFOConnection(std::string sendFilename, std::string receiveFilename, bool create = false);

//! create a local board
class IBoard *createBoard();

//! create a connection to a remote board
class IBoard *connectToBoard(class IConnection &connection);
