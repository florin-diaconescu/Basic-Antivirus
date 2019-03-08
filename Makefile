all: client server

link_emulator/lib.o:
	$(MAKE) -C link_emulator

client: client.o link_emulator/lib.o
	gcc -g client.o link_emulator/lib.o -o client

server: server.o link_emulator/lib.o
	gcc -g server.o link_emulator/lib.o -o server

client.o: client.c
	gcc -c client.c -o client.o

server.o: server.c
	gcc -c server.c -o server.o

clean:
	$(MAKE) -C link_emulator clean
	-rm -f *.o client server
