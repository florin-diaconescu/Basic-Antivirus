#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int send_file(char* file_name){
	//vreau sa transfer numele fisierului
	msg transfer;
	int size, file, read_check;

	memset(transfer.payload, 0, sizeof(transfer.payload));
	sprintf(transfer.payload, "%s", file_name);
	transfer.len = strlen(transfer.payload) + 1;
	send_message(&transfer);

	if (recv_message(&transfer) < 0){
		perror("Receive error!");
		return -1;
	}

	//vreau sa transfer dimensiunea fisierului
	file = open(file_name, O_RDONLY);
	if (file < 1){
		perror("File could not be opened!");
		return -1;
	}
	size = lseek(file, 0, SEEK_END);

	memset(transfer.payload, 0, sizeof(transfer.payload));
	sprintf(transfer.payload, "%d", size);
	transfer.len = strlen(transfer.payload) + 1;
	send_message(&transfer);

	if (recv_message(&transfer) < 0){
		perror("Receive error!");
		return -1;
	}

	//vreau sa transfer fisierul efectiv

	lseek(file, 0, SEEK_SET);

	memset(transfer.payload, 0, sizeof(transfer.payload));
	while ((read_check = read(file, transfer.payload, MAX_LEN)) > 0){
		transfer.len = read_check;
		send_message(&transfer);

		if (recv_message(&transfer) < 0){
			perror("Receive error!");
			return -1;
		}
	}

	close(file);
	return 0;
}

void listdir(char *dir){

	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	char *subdir;

	//daca nu este director, atunci este un fisier simplu
	if ((dp = opendir(dir)) == NULL){
		send_file(dir);
		return;
	}

	while((entry = readdir(dp)) != NULL){
		//if (lstat(entry->d_name, &statbuf) == 0){	
			
			if (entry->d_type == DT_DIR){
				
				//ignor . si ..
				if (strcmp(".", entry->d_name) == 0 \
					|| strcmp("..", entry->d_name) == 0){
					continue;
				}
					
				//aloc spatiu pentru directorul parinte, "/" si null
				subdir = malloc(strlen(dir) + strlen(entry->d_name) + 2);

				strcpy(subdir, dir);
				strcat(subdir, "/");
				strcat(subdir, entry->d_name);
				//strcat(subdir, "/");

				listdir(subdir);
				free(subdir);
			}

			else{
				printf("%s/%s\n", dir, entry->d_name);
				//printf("%s\n", dir);
				//send_file(entry->d_name);
			}
		}
	//}

	closedir(dp);
}

int main(int argc, char** argv){
	init(HOST, PORT);

	listdir(argv[1]);
	
	return 0;
}