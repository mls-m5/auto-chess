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
To start the main game you need to start the main server.

Use `--port` flag to set which port to open. Standard is `1234`.

``` bash
./chess             # start the server
```

## Run agents
For each agent you need a new terminal window.

Specify server address with `--host [name]` `--port [port]`.

Standard server is localhost and standard port 1234.

``` bash
./agents/monkey     # start a monkey 1000 AI
```

``` bash
./agents/human      # start a human interface
```
When starting the human agent you input your moves manually. This is just for
testing.

Design pattern
============

Server:

```
Board --> RemoteAgent --> IConnection --> Implemented connection 
```

Client:

```
Agent --> RemoteBoard --> IConnection --> Implemented connection
```



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


Create a own agent
==========

The easiest way to create a own agent is to copy the `./agents/monkey.cpp` file
to a new file (for example `./agents/new_agent.cpp`) and then add the line

```
new_agent.src += %.cpp
```

in `./agents/Matmakefile`. Then run `make` in the root directory to make the project

