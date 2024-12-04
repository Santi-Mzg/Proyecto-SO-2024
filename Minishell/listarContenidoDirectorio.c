#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define MAXIMO 500

struct dirent *dire;

void main(int argc, char *argv[]){
	char dir[100];
	strncpy(dir, argv[1], sizeof(dir));
	DIR *d = opendir(dir);
	if (d == NULL){
		printf("El directorio no fue encontrado.\n");
	}
	else{
		while ((dire = readdir(d)) != NULL){//mientras se pueda leer listamos el contenido
			if(dire->d_name != ".." && dire->d_name != ".") 
				printf("%s | ", dire->d_name);
		}
		printf("\n");
	}
	closedir(d);
}
