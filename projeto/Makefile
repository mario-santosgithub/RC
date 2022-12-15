# Makefile - Projeto de Redes de Computadores - 2º Período 2021/22 - IST

all: player GS

player: common.o Client/client_main.o Client/client_tcp.o Client/client_udp.o
	gcc -o player common.o Client/client_main.o Client/client_tcp.o Client/client_udp.o

GS: Server/server_main.o Server/server_udp.o Server/server_tcp.o
	gcc -o GS common.o Server/server_main.o Server/server_udp.o Server/server_tcp.o

clean:
	rm -rf Client/*.o Server/*.o *.o player GS PLAYERFILES Server/GAMES Server/SCORES
