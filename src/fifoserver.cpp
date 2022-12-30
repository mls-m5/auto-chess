#include "fifoserver.h"
#include "factoryfunctions.h"
#include "fifoconnection.h"
#include "iconnection.h"
#include "iserver.h"
#include <stdexcept>
#include <string>
#include <sys/stat.h> // for mkfifo

FIFOServer::FIFOServer(const std::string &agent1r,
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

FIFOServer::~FIFOServer() {
    _waitMutex.try_lock();
    _waitMutex.unlock();

    remove(_agent1files.first.c_str());
    remove(_agent1files.second.c_str());
    remove(_agent2files.first.c_str());
    remove(_agent2files.second.c_str());
}

void FIFOServer::callback(CallbackT func) {
    _callback = func;
}

void FIFOServer::listen() {
    if (_callback) {
        _callback(std::make_unique<FIFOConnection>(_agent1files.second,
                                                   _agent2files.first));
        _callback(std::make_unique<FIFOConnection>(_agent2files.second,
                                                   _agent2files.first));
        _waitMutex.lock(); // Wait for better times
    }
    else {
        throw std::runtime_error("callback function was not defined is server");
    }
}
