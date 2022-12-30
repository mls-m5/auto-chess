// Copyright Mattias Larsson Sköld 2019

#include "factoryfunctions.h"
#include "iboard.h"
#include "iconnection.h"
#include "remoteboard.h"
#include <iostream>
#include <memory>
#include <random>
#include <thread>

using namespace std;

extern std::string monkey;

int main(int argc, char **argv) {
    // monkey utf-8 messes up formatting
    cout << monkey << " This is the monkey 1000 advanced chess AI! " << monkey
         << endl;

    // Setup a connection defined by program arguments
    auto connection = connect(argc, argv);
    auto board = std::make_unique<RemoteBoard>(*connection);

    // Device and generator for random numbers
    random_device device;
    mt19937 generator(device());

    // Generate a random name
    uniform_int_distribution<std::mt19937::result_type> nameDistribution(0,
                                                                         9999);
    PlayerNum player = board->connect(
        "monkey" + std::to_string(nameDistribution(generator)), "");

    if (!player) {
        cout << "sorry... dit not get a player seat, quit..." << endl;
        return 0;
    }

    cout << "You are " << ((player == PlayerNum::Player1) ? "white" : "black")
         << endl;
    board->state().print();

    if (player == 2) {
        cout << "waiting for other player..." << endl;
        board->wait(player);
    }

    size_t tries = 0;
    bool running = true;

    // Handle if the match is finished
    auto checkStatus = [&running, &board]() {
        auto status = board->matchStatus();
        if (status >= MatchStatus::WhiteWon) {
            running = false;
        }
        switch (status) {
        case MatchStatus::Chess:
            cout << "Chess" << endl;
            break;
        case MatchStatus::WhiteWon:
            cout << "White won" << endl;
            break;
        case MatchStatus::BlackWon:
            cout << "Black won" << endl;
            break;
        case MatchStatus::Draw:
            cout << "Draw" << endl;
            break;
        default:
            break;
        }
    };

    // Super advanced AI stuff:

    // By trying on a local copy we can save network traffic
    auto localState = board->state();
    uniform_int_distribution<std::mt19937::result_type> distribution(0, 7);
    while (running) {
        auto fromX = distribution(generator);
        auto fromY = distribution(generator);
        auto toX = distribution(generator);
        auto toY = distribution(generator);
        ++tries;

        if (localState.isMoveValid(fromX, fromY, toX, toY, player) &&
            board->move(fromX, fromY, toX, toY)) {
            localState = board->state();
            localState.print();

            cout << "move successful after only " << tries << " tries" << endl;
            tries = 0;
            checkStatus();
            this_thread::sleep_for(.5s); // Let's not overwork it
        }
    }

    return 0;
}

std::string monkey = "🐵🐵🐵";



