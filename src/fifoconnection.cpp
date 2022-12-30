// Copyright Mattias Larsson Sköld 2019

#include "iconnection.h"
#include "iserver.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/stat.h> // for mkfifo

using namespace std;

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
            _output << endl;
        }
        _output.flush();
        return *this;
    }

    std::string read() override {
        openInputFile();
        if (!_input) {
            return "";
        }
        string line;
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
            throw runtime_error("could not open input fifo " + _inputFilename);
        }
    }

    void openOutputFile() {
        if (_output.is_open()) {
            return;
        }
        if (!std::experimental::filesystem::exists(_outputFilename)) {
            throw runtime_error("output fifo does not exist " +
                                _outputFilename);
        }
        _output.open(_outputFilename);
        if (!_output.is_open()) {
            throw runtime_error("could not open output fifo " +
                                _outputFilename);
        }
    }

    string _inputFilename;
    string _outputFilename;

    ifstream _input;
    ofstream _output;
};

class FIFOServer : public IServer {
public:
    FIFOServer(const std::string &agent1r,
               const std::string &agent1s,
               const std::string &agent2r,
               const std::string &agent2s)
        : _agent1files{agent1s, agent1r}
        , _agent2files{agent2s, agent2r} {

        mkfifo(_agent1files.first.c_str(), 0);
        mkfifo(_agent1files.second.c_str(), 0);
        mkfifo(_agent2files.first.c_str(), 0);
        mkfifo(_agent2files.second.c_str(), 0);
        _waitMutex.lock();
    }

    ~FIFOServer() {
        _waitMutex.try_lock();
        _waitMutex.unlock();

        remove(_agent1files.first.c_str());
        remove(_agent1files.second.c_str());
        remove(_agent2files.first.c_str());
        remove(_agent2files.second.c_str());
    }

    void callback(std::function<void(class IConnection *)> func) override {
        _callback = func;
    }

    void listen() override {
        if (_callback) {
            _callback(
                new FIFOConnection(_agent1files.second, _agent2files.first));
            _callback(
                new FIFOConnection(_agent2files.second, _agent2files.first));
            _waitMutex.lock(); // Wait for better times
        }
        else {
            throw std::runtime_error(
                "callback function was not defined is server");
        }
    }

    pair<string, string> _agent1files;
    pair<string, string> _agent2files;
    mutex _waitMutex;
    function<void(class IConnection *)> _callback;
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

class IServer *createFIFOServer(const std::string &agent1s,
                                const std::string &agent1r,
                                const std::string &agent2s,
                                const std::string &agent2r) {
    return new FIFOServer(agent1s, agent1r, agent2s, agent2r);
}
