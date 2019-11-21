// Copyright Mattias Larsson Sköld

#pragma once

#include <string>

class IConnection {
public:
	virtual ~IConnection() {}

	virtual void sendLine(const std::string &line) = 0;
	virtual std::string readLine() = 0;
	virtual IConnection &readLine(std::string &line) = 0;
	virtual operator bool() = 0;
};

