all: player SV

player: common.o client/main_client.o client/tcp_client.o client/udp_client.o
			gcc -o player common.o client/main_client.o client/tcp_client.o client/udp_client.o

SV: server/main_server.o server/udp_server.o server/tcp_server.o
		gcc -o SV common.o server/main_server.o server/udp_server.o server/tcp_server.o

clean: 
		rm -rf *.o client/*.o server/*.o player SV FILES