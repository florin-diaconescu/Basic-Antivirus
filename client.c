// Diaconescu Florin

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
#define MAX_FILE_NAME 100

int send_file(char* file_name){
	/* ---------------------------------------------------------- */
	//vreau sa transfer numele fisierului
	msg transfer;
	int size, file, read_check;

	memset(transfer.payload, 0, sizeof(transfer.payload));
	sprintf(transfer.payload, "%s", file_name);
	transfer.len = strlen(transfer.payload) + 1;
	send_message(&transfer);

	//afiseaza un mesaj de eroare la stdout daca nu s-a reusit receptia
	if (recv_message(&transfer) < 0){
		perror("Receive error!");
		return -1;
	}

	/* ---------------------------------------------------------- */
	//vreau sa transfer dimensiunea fisierului
	file = open(file_name, O_RDONLY);
	if (file < 1){
		perror("File could not be opened!");
		return -1;
	}

	//mut cursorul la finalul fisierului, ca sa calculez dimensiunea acestuia
	size = lseek(file, 0, SEEK_END);

	memset(transfer.payload, 0, sizeof(transfer.payload));
	sprintf(transfer.payload, "%d", size);
	transfer.len = strlen(transfer.payload) + 1;
	send_message(&transfer);

	if (recv_message(&transfer) < 0){
		perror("Receive error!");
		return -1;
	}

	/* ---------------------------------------------------------- */
	//vreau sa transfer fisierul efectiv, mutand initial cursorul la
	//inceputul fisierului si apoi trimitandu-l pe bucati de maxim 
	//1400 de bytes 
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

	//ultimul mesaj va fi corespunzator scanarii realizate de server
	if (recv_message(&transfer) < 0){
			perror("Receive error!");
			return -1;
	}

	close(file);
	//returnez valoarea convertita la int a scanarii
	return atoi(transfer.payload);
}

void listdir(char *dir){

	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	char *subdir;
	char *file_name = malloc(MAX_FILE_NAME);

	//daca nu este director, atunci este un fisier simplu
	if ((dp = opendir(dir)) == NULL){
		send_file(dir);
		return;
	}

	//pentru a putea scana si fisierele si subdirectoarele unui director,
	//apelez recursiv functia pana ce ajung la "frunze", adica fisierele
	//ce vor trebui scanate de server
	while((entry = readdir(dp)) != NULL){
		if (entry->d_type == DT_DIR){				
			//ignor . si ..
			if (strcmp(".", entry->d_name) == 0 \
				|| strcmp("..", entry->d_name) == 0){
				continue;
			}
					
			//aloc spatiu pentru directorul parinte, "/" si null
			subdir = malloc(strlen(dir) + strlen(entry->d_name) + 2);

			//construiesc string-ul corespunzator noului director parinte
			strcpy(subdir, dir);
			strcat(subdir, "/");
			strcat(subdir, entry->d_name);

			listdir(subdir);
			free(subdir);
		}

		else{
			//construiesc numele fisierului ce va trebui scanat, apoi
			//il trimit server-ului, pentru al scana
			sprintf(file_name, "%s/%s", dir, entry->d_name);
			if (send_file(file_name) == 0){
				
				printf("Fisierul %s este curat!\n", file_name);
			}
			else{
				printf("Fisierul %s este infectat!\n", file_name);
			}
			free(file_name);
		}
	}

	closedir(dp);
}

int main(int argc, char** argv){
	init(HOST, PORT);

	//pot apela antivirus-ul fie cu un fisier, fie cu un director,
	//asa ca am nevoie sa verific toate posibilele fisiere din acel
	//director
	listdir(argv[1]);
	
	return 0;
}