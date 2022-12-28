Computer Networks projet 2022-2023
Group 63

The Server directory contains the files needed for the execution of the server.
The same applies to the Client directory.

To run the Server type ./GS <wordFile> [-p port] [-v]  -> default port: 58063
To run the Client type ./player [-n ip] [-p port]      -> default ip: localhost

The Server will create directories SCORE and GAMES while executing, aswell as other complementary ones (PLID directories).
The files received by the player will be stored in a sub directory, PLAYERFILES, inside the Client.

All the additional directories are deleted when "make clean" command is executed.
To compile the project, simply type "make".