all: client server

link_emulator/lib.o:
	$(MAKE) -C link_emulator


client: client.o link_emulator/lib.o
	gcc -g client.o link_emulator/lib.o -o client

server: server.o link_emulator/lib.o plugins/string_scan.o plugins/binary_scan.o
	gcc -g server.o link_emulator/lib.o plugins/string_scan.o plugins/binary_scan.o -o server

client.o: client.c
	gcc -c client.c -o client.o

server.o: server.c
	gcc -c server.c -o server.o

plugins/string_scan.o: plugins/string_scan.c
	gcc -c plugins/string_scan.c -o plugins/string_scan.o

plugins/binary_scan.o: plugins/binary_scan.c
	gcc -c plugins/binary_scan.c -o plugins/binary_scan.o

clean:
	$(MAKE) -C link_emulator clean
	-rm -f *.o client server
