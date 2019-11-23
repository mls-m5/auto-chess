/*
 * iserver.h
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <functional>

class IServer {
public:
	virtual ~IServer() {}

	//! A callback that is called whenever a new connection is created
	virtual void callback(std::function<void(class IConnection*)>) = 0;

	//! Wait and listen for new connections
	virtual void listen() = 0;
};


