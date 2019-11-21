// Copyright Mattias Larsson Sköld 2019


#include "iconnection.h"
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

using namespace std;

class FIFOConnection: public IConnection {
public:
	FIFOConnection(std::string sendFilename, std::string receiveFilename):
		_inputFilename(receiveFilename),
		_outputFilename(sendFilename)
	{
		// Because opening files can be blocking the files is opened later
	}

	void sendLine(const std::string &line) override {
		openOutputFile();
//		cout << "sending line: " << line; cout.flush();
		_output << line;
		_output.flush();
	}

	std::string readLine() override {
		openInputFile();
		string line;
		getline(_input, line);

//		cout << "got line: " << line << endl;
		return line;
	}

	IConnection &readLine(std::string &line) override {
		line = readLine();
		return *this;
	}

	operator bool() override {
		return _input.is_open() && !_input.eof();
	}

private:

	void openInputFile() {
		if (_input.is_open()) {
			return;
		}
		_input.open(_inputFilename);
		if (!_input.is_open()) {
			throw runtime_error("could not open input fifo " + _inputFilename);
		}
	}

	void openOutputFile() {
		if (_output.is_open()) {
			return;
		}
		if (!std::experimental::filesystem::exists(_outputFilename)) {
			throw runtime_error("output fifo does not exist " + _outputFilename);
		}
		_output.open(_outputFilename);
		if (!_output.is_open()) {
			throw runtime_error("could not open output fifo " + _outputFilename);
		}
	}

	string _inputFilename;
	string _outputFilename;

	ifstream _input;
	ofstream _output;
};

IConnection *createFIFOConnection(std::string sendFilename, std::string receiveFilename, bool create = false) {
	if (create) {
		try {
			system(("mkfifo "+ sendFilename).c_str());
		}
		catch (...) {
			cout << "fifo " + sendFilename + " already created" << endl;
		}
		try {
			system(("mkfifo " + receiveFilename).c_str());
		}
		catch (...) {
			cout << "fifo " + receiveFilename + " already created" << endl;
		}
	}
	return new FIFOConnection(sendFilename, receiveFilename);
}
