Some quick notes, Open the server client first, and then open 2 clients
the included makefile will generate an executable for both the server and the client by default. Server is ./gameroom and client is ./client
I have included some previously generated leaderboard results to demonstrate that feature
the program can generate a leaderboard on it's own, but the leaderBoard file MUST BE FULLY DELETED AND NOT JUST WIPED
sometimes the client input gets stuck. If it does get stuck, make sure the server isn't waiting on the other client to do something first
if it's still stuck, smashing enter a bunch seems to have helped for memccpy
the server adress and ports are hard programmed into the code as local host just so that you don't have to tweak anything to get it to work on any machine
you can input the ports mannually into the client, but the server is only set up to listen on 2 specific ports, which the clients will default to if you dont
give them any arguements

This is a VERY rough version of this project.

Upcoming improvments:
change the server to allow for more than 1 game to be played at a time 
get rid of localhost hard coding, take host IP and port as arguements to the client
general robustness improvments, specifically with client side hangups and leaderboard error handling
rename most variable to be more descriptive
