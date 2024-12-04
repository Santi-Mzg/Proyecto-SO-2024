#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAXIMO 500

void main(int argc, char *argv[]){
	char dir[100];
	strncpy(dir, argv[1], sizeof(dir));
	int resultado = rmdir(dir);
	if(resultado == 0)
		printf("Directorio eliminado correctamente");
	else
		perror("Error al eliminar el directorio");
	
}
