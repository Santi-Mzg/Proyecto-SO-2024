#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define MAXIMO 500

struct dirent *dire;

void ayuda(){

	printf("mkdir nombre: Crea un directorio con el nombre indicado en la ubicacion actual.\n");
	printf("rmdir nombre : Elimina el directorio con ese nombre indicado.\n");
	printf("mkfile nombre: Crea un archivo con el nombre indicado en la dirección actual.\n");
	printf("ls nombre: Lista el contenido del directorio con el nombre indicado.\n");
	printf("showfile nombre: Muestra el contenido del archivo con el nombre indicado.\n");
	printf("fileperm nombre permisos : Cambia los permisos indicados de un archivo en el directorio con ese nombre.\n");
	printf("exit : Sale de la Minishell.\n");

}

void crearDirectorio(char dir[]){
	int resultado;
	resultado = mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(resultado == 0)
		printf("Directorio creado correctamente");
	else
		perror("Error al crear el directorio");
}

void eliminarDirectorio(char dir[]){
	rmdir(dir);
	printf("Directorio eliminado correctamente");
}

void crearArchivo(char file[]){
	FILE *f;
	f = fopen(file, "w+");
	if(f != NULL)
		printf("Archivo creado correctamente");
	else
		perror("Error al crear el Archivo");
	fclose(f);
}

void listarContenidoDirectorio(char dir[]){
	DIR *d = opendir(dir);
	if (d == NULL){
		printf("El directorio no fue encontrado.\n");
	}
	else{
		while ((dire = readdir(d)) != NULL){//mientras se pueda leer listamos el contenido
			printf("%s | ", dire->d_name);
		}
		printf("\n");
	}
	closedir(d);
}

void mostrarContenidoArchivo(char dir[]){
	//se abre el archivo en modo lectura
	FILE *f = fopen(dir, "r");
	char lee[MAXIMO];
	if (f != NULL){
		//mientras se pueda leer del archivo imprimimos el contenido 
		while (fgets(lee, MAXIMO, f) != NULL){
			printf(lee);
		}
	}
	else{
		printf("El archivo no fue encontrado");
	}
	printf("\n");
}

void modificarPermisos(char dir[], int p){	
	chmod(dir, p);
	printf("permisos modificados correctamente");
}

void limpiar(){    
	while (getchar()!= '\n'); 
}

int main(){
	char comando[MAXIMO];
	char d[MAXIMO];
	int val;

	printf("Bienvenido a la Minishell: Para ver los comandos utilice 'help'. \n\n");

	//mientras no elija salir
	while (strcmp("exit", comando) != 0){
		//lee el comando
		fflush(stdin);
		printf("- ");
		scanf("%s", comando);

		//ejecuta el comando elegido
		if (strcmp("mkdir", comando) == 0){
			scanf("%s",d);
			crearDirectorio(d);
		}
		else if(strcmp("rmdir",comando) == 0){
			scanf("%s", d);
			eliminarDirectorio(d);
			limpiar();
		}
		else if(strcmp("mkfile",comando) == 0){
			scanf("%s", d);
			crearArchivo(d);
			limpiar();
		}
		else if(strcmp("ls",comando) == 0){
			scanf("%s", d);
			listarContenidoDirectorio(d);
			limpiar();
		}
		else if(strcmp("showfile", comando) == 0){
			scanf("%s", d);
			mostrarContenidoArchivo(d);
			limpiar();
		}
		else if(strcmp("help",comando) == 0){
			ayuda();
		}
		else if(strcmp("fileperm",comando) == 0){
			scanf("%s", d);
			scanf("%d", &val);
			modificarPermisos(d, val);
			limpiar();
		}
		else if(strcmp("exit",comando) == 0){
			printf("\n Ha salido del MiniShell.\n");
		}
		else{
			printf("Comando incorrecto, utilice 'help' para conocer los comandos.\n\n");
		}

		printf("\n");
	
	}

	return 0;
}
