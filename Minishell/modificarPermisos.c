#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAXIMO 500

void main(int argc, char *argv[]){	
	char dir[100];
	strncpy(dir, argv[1], sizeof(dir));
 	int valor = atoi(argv[2]);
 	chmod(dir, valor);
	printf("permisos modificados correctamente");
}
