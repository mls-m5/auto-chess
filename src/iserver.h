/*
 * iserver.h
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <functional>
#include <memory>

class IServer {
public:
    using CallbackT = std::function<void(std::unique_ptr<class IConnection>)>;

    virtual ~IServer() {}

    //! A callback that is called whenever a new connection is created
    virtual void callback(CallbackT) = 0;

    //! Wait and listen for new connections
    virtual void listen() = 0;
};
