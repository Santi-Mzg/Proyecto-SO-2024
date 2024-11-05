#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * open_file(char *filename){
    FILE * file;
    file = fopen(filename, "a");
    if(file == NULL)
    {
        printf("No es posible crear archivo.");
        exit(EXIT_FAILURE);
    }
    return file;
}

int generar_numero(int pipe_generador[],int id_gen){
    close(pipe_generador[0]);
    srand(getpid());
    int random_int;
    do{
      random_int = rand() % 100;
      printf("%d generado por gen %d \n", random_int, id_gen);
      fflush(stdout);
      write(pipe_generador[1], &random_int, sizeof(int));
      sleep(2);
    }while(1);
}

int escribir_numero(int pipe_salida[],int id_escr){
    close(pipe_salida[1]);
    char filename[] = "SalidaX.txt";
    filename[6] = id_escr + '0';
    FILE * file;
    
    int num;
    do{
      file = open_file(filename);
      read(pipe_salida[0], &num, sizeof(int));
      printf("%d escrito por %d\n", num, id_escr);
      fflush(stdout);
      fprintf(file, "%d \n", num);
      fclose(file);
      sleep(1);
    }while(1);
    exit(0);
}

int proceso_control(int pipe_control[]){
    close(pipe_control[0]);
    char msg[] = "OK";
    do{
      fflush(stdout);
      write(pipe_control[1], &msg, sizeof(char)*3);
      sleep(7);
    }while(1);
}

 
int main( int argc, char **argv ){
  pid_t pid;
  int salida = 0; // 0 -> salida 1, 1 -> salida 2
  int pipe_generador[2], pipe_salida1[2], pipe_salida2[2], pipe_control[2];
  
  pipe(pipe_generador);
  pipe(pipe_salida1);
  pipe(pipe_salida2); 
  pipe(pipe_control);
 
  if ( (pid=fork()) == 0 ){ 
    // hijo generador 1
    generar_numero(&pipe_generador[0], 1);
  }
  if ( (pid=fork()) == 0 ){ 
    // hijo generador 2
    generar_numero(&pipe_generador[0], 2);
  }
  if ( (pid=fork()) == 0 ){ 
    // hijo escritor 1
    escribir_numero(&pipe_salida1[0], 1);
  }
  if ( (pid=fork()) == 0 ){ 
    // hijo escritor 2
    escribir_numero(&pipe_salida2[0], 2);
  }
  if ( (pid=fork()) == 0 ){ 
    // hijo proceso_control
    fflush(stdout);
    proceso_control(&pipe_control[0]);
  }
  
  else{
    close(pipe_generador[1]);
    close(pipe_control[1]);
    close(pipe_salida1[0]);
    close(pipe_salida2[0]);
    fcntl(pipe_control[0], F_SETFL, O_NONBLOCK);
    int num;
    char control[3];
    do{
      read(pipe_generador[0], &num, sizeof(int));
      int read_control = read(pipe_control[0], &control, sizeof(char)*3);
      if(read_control != -1 && strcmp(control,"OK") == 0){
          salida = (salida + 1) % 2;
          printf("cambio de control, salida=%d\n", salida+1);
          fflush(stdout);
      }
      if(salida==0){
        write(pipe_salida1[1], &num, sizeof(int));
      }else{
        write(pipe_salida2[1], &num, sizeof(int));
      }
      sleep(1);
    }while(1);
  }
  while(wait(NULL) > 0);
  exit( 0 );
}