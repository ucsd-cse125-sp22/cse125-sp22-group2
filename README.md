# Team Big Squid Game Studios
Our game is Skrrt Skirt: a 3-versus-1 game of reverse tag in which one player possesses a crown and three other players try to steal that crown for themselves. 
The goal of the game is to possess the crown for the most amount of time.

[Link](https://cse125.ucsd.edu/2022/cse125g2/wordpress/index.php/home/) to our website

[Link](https://docs.google.com/presentation/d/1O8KoVTrBqRAX6rzp8X4amB2-P36l8V5ngvL5kg56-Uw/edit?usp=sharing) to our demo presentation

# Playing our Game
1. You will need to install [Visual Studio 2019](https://visualstudio.microsoft.com/vs/older-downloads/) on each computer that is running the game.
2. In the `config.json` file, specify the IP address and port number for the server.
3. Open `server/server.sln` using Visual Studio.
4. Build and run the `server.sln` project. Make sure that the IP address and port number of the computer running the server matches what is
in the `config.json` file.
6. Open `skrrt/Game.sln` using Visual Studio. 
7. If using 4 separate computers, build and run `Game.sln` on each.
8. If using just 1 computer, you can execute the `skrrt/Game/run4.sh` script from a terminal in the `Game` directory
to run 4 clients on one computer.
