#pragma once

#include "iconnection.h"
#include <fstream>

class FIFOConnection : public IConnection {
public:
    //! Create a named pipe connection
    //! if create is true new fifo files is created
    //! Can only be used on the same machine
    FIFOConnection(std::string sendFilename,
                   std::string receiveFilename,
                   bool create = false);

    IConnection &sendLine(const std::string &line) override;

    std::string read() override;

    IConnection &read(std::string &line) override;

    operator bool() override;

private:
    void openInputFile();

    void openOutputFile();

    std::string _inputFilename;
    std::string _outputFilename;

    std::ifstream _input;
    std::ofstream _output;
};
