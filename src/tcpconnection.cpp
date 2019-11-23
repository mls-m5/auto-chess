/*
 * tcpconnection.cpp
 *
 *  Created on: 23 nov. 2019
 *      Author: Mattias Larsson Sköld
 */

#include "factoryfunctions.h"

#include "iconnection.h"
#include "iserver.h"

#include <stdexcept>
#include <thread>
#include <array>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>



using namespace std;

namespace {

// Connect to a remote connection
class TCPConnection: public IConnection {
public:
	~TCPConnection() {
		::close(_socket);
	}

	TCPConnection(const std::string &hostname, short remotePort) {
		if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			throw std::runtime_error("could not create socket");
		}
		sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_port = htons(remotePort);

		// Resolve hostname
		if (inet_pton(AF_INET, hostname.c_str(), &address.sin_addr) < 0) {
			throw std::runtime_error("could not resolve host");
		}

		if (connect(_socket,
				reinterpret_cast<sockaddr*>(&address),
				sizeof(address)) < 0) {
			throw std::runtime_error("could not connect to remote computer");
		}
	}

	//! Create a TCPConnection from a existing socket
	TCPConnection(int socket) {
		_socket = socket;
	}


	IConnection &sendLine(const std::string &line) override {
		if (line.back() == '\n') {
			if (send(_socket, line.c_str(), line.size(), 0) <= 0) {
				close();
			}
		}
		else {
			auto stringWithNewline = line + "\n";
			if (send(_socket,
					stringWithNewline.c_str(),
					stringWithNewline.size(),
					0) <= 0) {
				close();
			}
		}
		return *this;
	}

	std::string read() {
		std::array<char, 1024> buffer;
		auto len = ::read(_socket, buffer.data(), buffer.size());
		if (len <= 0) {
			close();
			return "";
		}
		if (buffer[len - 1] == '\n') {
			--len;
		}
		return std::string(buffer.begin(), buffer.begin() + len);
	}

	IConnection &read(std::string &line) override {
		line = read();
		if (line.empty()) {
			throw runtime_error("connection is closed");
		}
		return *this;
	}

	operator bool() override {
		return _socket > -1;
	}

private:
	void close() {
		if (_socket > 0) {
			::close(_socket);
			_socket = -1;
		}
	}

	int _socket = -1;
};


class TCPServer: public IServer {
public:
	~TCPServer() {
		close(_socket);
	}

	TCPServer(short port) {
		if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			throw std::runtime_error("could not create socket");
		}

		int option = 1;

		if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				reinterpret_cast<void*>(&option), sizeof(option))) {
			throw ("failed to set socket options");
		}

		sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		// Non blocking flags
//		auto flags = fcntl(_socket, F_GETFL);
//		if (flags > -1) {
//			if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
//				throw runtime_error("failed setting listening socket to non-blocking");
//			}
//		}

		// Bind socket to port
		if (::bind(_socket,
				reinterpret_cast<sockaddr*>(&address),
				sizeof(address))) {
			throw runtime_error("failed to bind port " + to_string(port));
		}
	}


	// Listen and wait for clients to connect
	void listen() {
		size_t maxQueLen = 100;

		if (::listen(_socket, maxQueLen) < 0) {
			throw runtime_error("error when listening");
		}

		while (_active) {
			sockaddr_in address = {};
			socklen_t addressLength = sizeof(address);
			auto newSocket = accept(_socket,
					reinterpret_cast<sockaddr*>(&address),
					&addressLength);

			if (newSocket > -1) {
				// Add connection
				if(_callback) {
					_callback(new TCPConnection(newSocket));
				}
			}
			this_thread::sleep_for(.5s);
		}
	}

	void callback(std::function<void(class IConnection *)> func) {
		_callback = func;
	}

private:
	int _socket = 0;
	int _active = true;
	function<void(class IConnection *)> _callback;
};


}

IServer *createTCPServer(short port) {
	return new TCPServer(port);
}

IConnection *createTCPConnection(const std::string &hostname, short port) {
	return new TCPConnection(hostname, port);
}

