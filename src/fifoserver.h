#pragma once

#include "iserver.h"
#include <mutex>
#include <string>

class FIFOServer : public IServer {
public:
    FIFOServer(const std::string &agent1r,
               const std::string &agent1s,
               const std::string &agent2r,
               const std::string &agent2s);

    ~FIFOServer();

    void callback(CallbackT func) override;

    void listen() override;

private:
    std::pair<std::string, std::string> _agent1files;
    std::pair<std::string, std::string> _agent2files;
    std::mutex _waitMutex;
    CallbackT _callback;
};
