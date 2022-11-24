all: user DS

user: common.o Client/main_client.o Client/tcp_client.o Client/udp_client.o
	gcc -o user common.o Client/main_client.o Client/tcp_client.o Client/udp_client.o

DS: Server/main_server.o Server/udp_server.o Server/tcp_server.o
	gcc -o DS common.o Server/main_server.o Server/udp_server.o Server/tcp_server.o

clean:
	rm -rf Client/*.o Server/*.o *.o user DS DOWNLOADS

clean-server:
	rm -rf GROUPS USERS