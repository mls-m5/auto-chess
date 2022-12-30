// Copyright Mattias Larsson Sköld 2019

#include "fifoconnection.h"
#include "iconnection.h"
#include "iserver.h"
#include <experimental/filesystem>
#include <iostream>
#include <mutex>
#include <sys/stat.h> // for mkfifo

FIFOConnection::FIFOConnection(std::string sendFilename,
                               std::string receiveFilename,
                               bool create)
    : _inputFilename(receiveFilename)
    , _outputFilename(sendFilename) {

    if (create) {
        try {
            system(("mkfifo " + sendFilename).c_str());
        }
        catch (...) {
            std::cout << "fifo " + sendFilename + " already created"
                      << std::endl;
        }
        try {
            system(("mkfifo " + receiveFilename).c_str());
        }
        catch (...) {
            std::cout << "fifo " + receiveFilename + " already created"
                      << std::endl;
        }
    }
    // Because opening files can be blocking the files is opened later
}

IConnection &FIFOConnection::sendLine(const std::string &line) {
    openOutputFile();
    //		cout << "sending line: " << line; cout.flush();
    _output << line;
    if (line.back() != '\n') {
        _output << std::endl;
    }
    _output.flush();
    return *this;
}

std::string FIFOConnection::read() {
    openInputFile();
    if (!_input) {
        return "";
    }
    std::string line;
    getline(_input, line);

    //		cout << "got line: " << line << endl;
    return line;
}

IConnection &FIFOConnection::read(std::string &line) {
    line = read();
    return *this;
}

FIFOConnection::operator bool() {
    return _input.is_open() && !_input.eof();
}

void FIFOConnection::openInputFile() {
    if (_input.is_open()) {
        return;
    }
    _input.open(_inputFilename);
    if (!_input.is_open()) {
        throw std::runtime_error("could not open input fifo " + _inputFilename);
    }
}

void FIFOConnection::openOutputFile() {
    if (_output.is_open()) {
        return;
    }
    if (!std::experimental::filesystem::exists(_outputFilename)) {
        throw std::runtime_error("output fifo does not exist " +
                                 _outputFilename);
    }
    _output.open(_outputFilename);
    if (!_output.is_open()) {
        throw std::runtime_error("could not open output fifo " +
                                 _outputFilename);
    }
}
