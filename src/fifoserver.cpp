#include "factoryfunctions.h"
#include "iconnection.h"
#include "iserver.h"
#include <functional>
#include <mutex>
#include <stdexcept>
#include <string>
#include <sys/stat.h> // for mkfifo

namespace {

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

    void callback(CallbackT func) override {
        _callback = func;
    }

    void listen() override {
        if (_callback) {
            _callback(
                createFIFOConnection(_agent1files.second, _agent2files.first));
            _callback(
                createFIFOConnection(_agent2files.second, _agent2files.first));
            _waitMutex.lock(); // Wait for better times
        }
        else {
            throw std::runtime_error(
                "callback function was not defined is server");
        }
    }

    std::pair<std::string, std::string> _agent1files;
    std::pair<std::string, std::string> _agent2files;
    std::mutex _waitMutex;
    CallbackT _callback;
};

} // namespace

std::unique_ptr<class IServer> createFIFOServer(const std::string &agent1s,
                                                const std::string &agent1r,
                                                const std::string &agent2s,
                                                const std::string &agent2r) {
    return std::make_unique<FIFOServer>(agent1s, agent1r, agent2s, agent2r);
}
