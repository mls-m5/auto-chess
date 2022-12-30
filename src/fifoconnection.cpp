// Copyright Mattias Larsson Sköld 2019

#include "iconnection.h"
#include "iserver.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/stat.h> // for mkfifo

namespace {

class FIFOConnection : public IConnection {
public:
    FIFOConnection(std::string sendFilename, std::string receiveFilename)
        : _inputFilename(receiveFilename)
        , _outputFilename(sendFilename) {
        // Because opening files can be blocking the files is opened later
    }

    IConnection &sendLine(const std::string &line) override {
        openOutputFile();
        //		cout << "sending line: " << line; cout.flush();
        _output << line;
        if (line.back() != '\n') {
            _output << std::endl;
        }
        _output.flush();
        return *this;
    }

    std::string read() override {
        openInputFile();
        if (!_input) {
            return "";
        }
        std::string line;
        getline(_input, line);

        //		cout << "got line: " << line << endl;
        return line;
    }

    IConnection &read(std::string &line) override {
        line = read();
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
            throw std::runtime_error("could not open input fifo " +
                                     _inputFilename);
        }
    }

    void openOutputFile() {
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

    std::string _inputFilename;
    std::string _outputFilename;

    std::ifstream _input;
    std::ofstream _output;
};

} // namespace
class IConnection *createFIFOConnection(const std::string &sendFilename,
                                        const std::string &receiveFilename,
                                        bool create = false) {
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
    return new FIFOConnection(sendFilename, receiveFilename);
}
