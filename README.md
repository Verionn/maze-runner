# Maze Runner

Maze Runner is a multiplayer synchronized game made in C using the Ncurses library. The game allows up to 4 players to connect through a server-client connection using sockets.

The objective of the game is to collect as many coins as possible within a labyrinth. However, players must watch out for beasts that hunt them down and can kill them if they catch the player up. When a player is killed, their coins fall to the ground. To collect coins, players must navigate through the labyrinth, avoid beasts, and bring the collected coins to a designated base.

**You must have at least 4 consoles open, because the 4th is the one which receives messages and it needs to be open since the start of the server.
You can check the index of console by using  ***tty*** in the linux console.**

# Compile: 

Server:
gcc server.c -o server -pthread -lncurses

./server

Player:
gcc player.c -o player -lncurses

./player

# Server view
![image](https://user-images.githubusercontent.com/86195102/235660656-f4a9f09e-3b1f-4a62-8ed5-f4b2931ddaf0.png)

# Player view
![image](https://user-images.githubusercontent.com/86195102/235660740-d9d73ac7-e54f-4409-98bd-50a5bd27e5de.png)

# Full-Game view
![image](https://user-images.githubusercontent.com/86195102/235660174-7599b353-4488-4d74-96b4-55b729dd7bc3.png)
