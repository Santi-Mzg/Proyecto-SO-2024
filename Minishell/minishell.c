#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


#define MAXIMO 500

struct dirent *dire;

void limpiar(){    
	while (getchar()!= '\n'); 
}

int main(){
	char comando[MAXIMO];
	char d[MAXIMO];
	char val[100];

	printf("Bienvenido a la Minishell: Para ver los comandos utilice 'help'. \n\n");

	//mientras no elija salir
	while (strcmp("exit", comando) != 0){
		//lee el comando
		fflush(stdin);
		printf("-> ");
		scanf("%s", comando);

		//ejecuta el comando elegido
		if (strcmp("mkdir", comando) == 0){
			scanf("%s", &d);
			char *path = "./crearDirectorio";
			if(fork() == 0) {
				char *args[] = {"./crearDirectorio", d, NULL};
				execv(path, args);
				exit(0);
			}
		}
		else if(strcmp("rmdir",comando) == 0){
			scanf("%s", d);
			char *path = "./eliminarDirectorio";
			if(fork() == 0) {
				char *args[] = {"./eliminarDirectorio", d, NULL};
				execv(path, args);
				exit(0);
			}
			//execv();
			//eliminarDirectorio(d);
			//limpiar();
		}
		else if(strcmp("mkfile",comando) == 0){
			scanf("%s", d);
			char *path = "./crearArchivo";
			if(fork() == 0) {
				char *args[] = {"./crearArchivo", d, NULL};
				execv(path, args);
				exit(0);
			}
			//crearArchivo(d);
			//limpiar();
		}
		else if(strcmp("ls",comando) == 0){
			scanf("%s", d);
			char *path = "./listarContenidoDirectorio";
			if(fork() == 0) {
				char *args[] = {"./listarContenidoDirectorio", d, NULL};
				execv(path, args);
				exit(0);
			}			
			//listarContenidoDirectorio(d);
			//limpiar();
		}
		else if(strcmp("showfile", comando) == 0){
			scanf("%s", d);
			char *path = "./mostrarContenidoArchivo";
			if(fork() == 0) {
				char *args[] = {"./mostrarContenidoArchivo", d, NULL};
				execv(path, args);
				exit(0);
			}	
			//mostrarContenidoArchivo(d);
			//limpiar();
		}
		else if(strcmp("help",comando) == 0){
			char *path = "./ayuda";
			if(fork() == 0) {
				char *args[] = {"./ayuda", d, NULL};
				execv(path, args);
				exit(0);
			}
			//ayuda();
		}
		else if(strcmp("fileperm",comando) == 0){
			scanf("%s", d);
			scanf("%s", val);
			char *path = "./modificarPermisos";
			if(fork() == 0) {
				char *args[] = {"./modificarPermisos", d, val, NULL};
				execv(path, args);
				exit(0);
			}
			//modificarPermisos(d, val);
			//limpiar();
		}
		else if(strcmp("exit",comando) == 0){
			printf("\n Ha salido del MiniShell.\n");
		}
		else{
			printf("Comando incorrecto, utilice 'help' para conocer los comandos.\n\n");
		}
		wait(NULL);
		printf("\n");
	
	}

	return 0;
}
