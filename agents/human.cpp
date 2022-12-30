// Copyright Mattias Larsson Sköld 2019

#include "factoryfunctions.h"
#include "iboard.h"
#include "iconnection.h"
#include "remoteboard.h"
#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

void printInvalidInfo() {
    cout << "invalid move" << endl;
    cout << "format: [from] [to]" << endl;
    cout << "eg: a1 a3" << endl;
}

int main(int argc, char **argv) {
    cout << "Welcome human!" << endl;

    auto connection = connect(argc, argv);
    auto board = std::make_unique<RemoteBoard>(*connection);

    bool running = true;

    auto player = board->connect("", "");

    if (player) {
        cout << "You are "
             << ((player == PlayerNum::Player1) ? "white" : "black") << endl;
    }

    board->state().print();

    if (player == 2) {
        cout << "waiting for other player..." << endl;
        board->wait(player);
        cout << "your turn" << endl;
        board->state().print();
    }
    else if (player == 0) {
        cout << "You are spectator" << endl;
        while (*connection) {
            board->state().print();
            board->wait(PlayerNum::None);
        }
    }

    string line;
    MatchStatus status = MatchStatus::Normal;
    while (getline(cin, line)) {
        if (line[0] >= 'a' && line[0] <= 'h') {
            istringstream ss(line);
            string from;
            string to;
            if (ss >> from) {
                ss >> to;
            }
            else {
                printInvalidInfo();
                continue;
            }

            if (from.size() != 2 || to.size() != 2) {
                printInvalidInfo();
                continue;
            }

            int fromX = from[0] - 'a';
            int fromY = from[1] - '1';

            int toX = to[0] - 'a';
            int toY = to[1] - '1';

            if (!board->move(fromX, fromY, toX, toY)) {
                board->state().print();

                cout << "illegal move, try again" << endl;
            }
            else {
                board->state().print();

                status = board->matchStatus();

                if (status <= MatchStatus::Chess) {
                    cout << "waiting for other player..." << endl;
                    board->wait(player);

                    status = board->matchStatus();
                }
            }
        }

        board->state().print();

        switch (status) {
        case MatchStatus::Normal:
            cout << "make your move" << endl;
            break;
        case MatchStatus::Chess:
            cout << "Chess!" << endl;
            break;
        case MatchStatus::WhiteWon:
            board->state().print();
            cout << "White won!" << endl;
            return 0;
        case MatchStatus::BlackWon:
            board->state().print();
            cout << "Black won!" << endl;
            return 0;
        case MatchStatus::Draw:
            board->state().print();
            cout << "It is a draw" << endl;
        }
    }
}
