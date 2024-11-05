#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAXIMO 500

void main(int argc, char *argv[]){

	printf("mkdir nombre: Crea un directorio con el nombre indicado en la ubicacion actual.\n");
	printf("rmdir nombre : Elimina el directorio con ese nombre indicado.\n");
	printf("mkfile nombre: Crea un archivo con el nombre indicado en la dirección actual.\n");
	printf("ls nombre: Lista el contenido del directorio con el nombre indicado.\n");
	printf("showfile nombre: Muestra el contenido del archivo con el nombre indicado.\n");
	printf("fileperm nombre permisos : Cambia los permisos indicados de un archivo en el directorio con ese nombre.\n");
	printf("exit : Sale de la Minishell.\n");

}
