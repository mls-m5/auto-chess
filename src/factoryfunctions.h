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
std::unique_ptr<class IConnection> connect(int argc, char **argv);

//! Create a generic server
//! Different type of server will be generated depending on arguments
std::unique_ptr<class IServer> createServer(int argc, char **argv);
