#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"
#include "plugins/stringscan.h"
#include "plugins/binaryscan.h"

#define HOST "127.0.0.1"
#define PORT 10001
//#define OUTPUT_DIR "antivirus_files//"

int main(int argc, char** argv){
	init(HOST, PORT);

	msg transfer, receive;
	int size, count = 0, file;
	char* received_file_name = malloc(50);
	char* new_file_name = malloc(50);

	//vreau sa verific primirea numelui fisierului
	if (recv_message(&receive)<0){
    	perror("Send error!");
    	return -1;
  	}

	//salvez numele fisierului ce va fi primit
  	memcpy(received_file_name, receive.payload, receive.len);

  	sprintf(transfer.payload, "%s", receive.payload);
  	transfer.len = strlen(transfer.payload) + 1;
  	send_message(&transfer);

  	//vreau sa verific primirea dimensiunii fisierului
  	if (recv_message(&receive)<0){
    	perror("Send error!");
    	return -1;
  	}

  	sprintf(transfer.payload, "%s", receive.payload);
  	transfer.len = strlen(transfer.payload) + 1;
  	send_message(&transfer);

  	//salvez dimensiunea fisierului si deschid fisierul de iesire
  	size = atoi(receive.payload);

  	//sprintf(new_file_name, OUTPUT_DIR"%s", received_file_name);

  	file = open("scan_file", O_WRONLY | O_CREAT, 0644);
  	if (file < 0){
  		perror("File could not be created!");
  		return -1;
  	}

  	while (count < size){
  		if (recv_message(&receive)<0){
    		perror("Send error!");
    		return -1;
  		}

  		count += write(file, receive.payload, receive.len);

  		sprintf(transfer.payload, "%s", receive.payload);
  		transfer.len = strlen(transfer.payload) + 1;
  		send_message(&transfer);
  	}

  	close(file);
  	return 0;
}