#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAXIMO 500

void main(int argc, char *argv[]){	
	char dir[100];
	char *endptr = NULL;
	strncpy(dir, argv[1], sizeof(dir));
 	int valor = atoi(argv[2]);
 	int modo = strtol(argv[2], &endptr, 8);
 	chmod(dir, modo);
	printf("Permisos modificados correctamente");
}

