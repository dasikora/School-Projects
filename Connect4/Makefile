CC = gcc

all: gameroom client

gameroom: gameroom.c
	$(CC) -c -o gameroom.o gameroom.c
	$(CC) -o gameroom gameroom.o
	
client: client.c
	$(CC) -c -o client.o client.c
	$(CC) -o client client.o