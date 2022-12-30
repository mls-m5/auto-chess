// Copyright Mattias Larsson Sköld

#pragma once

#include <string>

//! A abstract connection used to connect remote boards to agents and vice versa
class IConnection {
public:
    virtual ~IConnection() {}

    //! guaranteed to send a '\n' at end of transmission
    virtual IConnection &sendLine(const std::string &line) = 0;

    //! read a single line, and wait if there is no line to read
    virtual std::string read() = 0;

    //! the same as readLine() but returns a reference to the connection
    //! to be used in for example if or while-statement
    virtual IConnection &read(std::string &line) = 0;

    //! Returns the state of the connection
    //! returns true if the connection is still active and
    //! false if it is closed or end of file is reached
    virtual operator bool() = 0;
};
