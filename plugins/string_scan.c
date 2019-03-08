// Diaconescu Florin

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "string_scan.h"

#define malicious_word "virus"
#define BUFFER_SIZE 100

int string_scan(char *file_name){
	int file;
	char *buffer;
	size_t bufsize = BUFFER_SIZE;
	size_t characters;

	buffer = (char *) malloc(bufsize * sizeof(char));
	if (buffer == NULL){
		perror("Unable to alocate buffer!");
		return -1;
	}

	file = open(file_name, O_RDONLY);
	if (file < 0){
		perror("File could not be opened!");
		return -1;
	}

	//citeste bucati de BUFFER_SIZE si verifica prezenta unui string malitios,
	//functionalitate ce poate fi evident extinsa la un vector de string-uri
	//sau un fisier de tip dictionar oferit spre parsare
	while (characters = read(file, buffer, BUFFER_SIZE) > 0){
		if (strstr(buffer, malicious_word) != NULL){
			free(buffer);
			return 1;
		}
	}

	free(buffer);
	return 0;
}
