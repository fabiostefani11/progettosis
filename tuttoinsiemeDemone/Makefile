all: server client 

server: server.o funzioniServer.o thpool.o
	gcc -g -o server server.o funzioniServer.o thpool.o -lpthread -lrt

server.o: server.h server.c thpool.h
	gcc -g -c server.c

funzioniServer.o: funzioniServer.c
	gcc -g -c funzioniServer.c

client: client.o
	gcc -g -o client client.o

client.o: client.c
	gcc -g -c client.c

thpool.o: thpool.h thpool.c
	gcc -g -c thpool.c 
