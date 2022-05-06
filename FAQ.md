# Where do I specify config info?
In the top level of this repository, in the file `config.json`.

# How do I change the host's IP address and port number?
In `config.json`, edit the `host` and `port` key-value pairs.

For example, if you wanted to run the game on 1 computer:

`"host": "localhost",
"port": "8000"`.

To run the game on multiple machines in the lab, set `host` to be
the IP address of the computer you want to run the server on.

# How do I change the number of players in the game?
In the `Constants.hpp` file, modify the `NUM_PLAYERS` variable.
Don't forget to build the client and server executables before running them again!

# How can I quickly run 4 clients at the same time?
From a terminal in the `skrrt/Game/` directory, execute the `run4.sh` scripts by doing `./run4.sh`.
