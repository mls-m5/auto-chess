Auto-chess
=========


A framework for training/testing AI-agents in playing chess.


How to build
============

## Linux
make sure to have g++ (or clang) make and git installed.

```bash
git clone https://github.com/mls-m5/auto-chess

cd auto-chess

git submodule update --init

make -j

```


# Run main application
``` bash
./chess
```


Design pattern
============

## Agents
An agent is a human or computer that acts on the board (a player)

## IBoard
The IBoard is a interface the agents can use to interact with the board. The interface could be used when one or both players play over remote connection, but also when playing within the same application.

## Board
In the server application the Board handles player moves and printing status (if the agents are interested).

## Remote board connection
A connection to a board on a server. The server can of course be on the same computer or a remote computer.

## Remote client connection
A connection used by a server to communicate with remote agents.

