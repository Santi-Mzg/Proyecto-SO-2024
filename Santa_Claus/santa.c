#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h>
#include <time.h>
#include <unistd.h> 

#define CANT_RENOS 9
#define CANT_ELFOS 5
#define CANT_GRUPO_ELFOS 3

sem_t elfosOrenos, renosListos, elfosConProblemas, renosEnPoloNorte, renosEnTropico, armarTrineo, santaAyudaElfo, grupoDe3Elfos, renosParaDespertarASanta, elfosParaDespertarASanta; 

pthread_mutex_t mutexRenos, mutexElfos1, mutexElfos2;

void *santa(void *arg) { 
    while (1) { 
        printf("Santa se va a dormir\n");
        sem_wait(&elfosOrenos);
        
        if(sem_trywait(&renosListos) == 0){
            printf("Se arma el trineo\n");
            sleep(5);
            printf("Santa terminó de prepara el trineo y se fue\n");
            sleep(15);
            printf("Santa y los renos volvieron\n");

            for(int i = 0; i < CANT_RENOS; i++){
                sem_post(&armarTrineo); 
            }
        }
        else {
            if(sem_trywait(&elfosConProblemas) == 0){ 
                printf("Santa ayuda a los elfos\n");
                sleep(5);
                printf("Santa terminó de ayudar a los elfos\n");
                
                for(int i = 0; i < CANT_GRUPO_ELFOS; i++){
                    sem_post(&santaAyudaElfo); 
                }
            }
        }
    } 
} 

void *reno(void *arg) { 
    int id = *((int*) arg);
    int time;
    
    while (1) { 
        
        printf("Reno %d en el TROPICO\n", id); 
        time = rand()%15 + 10;
        sleep(time);
        
        sem_wait(&renosEnTropico); // Ve si se puede unir al grupo de renos
        
        pthread_mutex_lock(&mutexRenos);
        sem_post(&renosEnPoloNorte); 

        if(sem_trywait(&renosEnTropico) != 0){ // Si es el ultimo reno
            sem_post(&renosListos); // Avisa a santa que lo necesitan los renos
            sem_post(&elfosOrenos); // Despierta a santa
            printf("ULTIMO Reno %d va al POLO NORTE y avisa a Santa\n", id);
        }
        else {
            printf("Reno %d va al POLO NORTE\n", id);
            sem_post(&renosEnTropico);
        }
        pthread_mutex_unlock(&mutexRenos);

        sem_wait(&armarTrineo); // Espera a que santa arme el trineo y viajen juntos

        pthread_mutex_lock(&mutexRenos);
        sem_wait(&renosEnPoloNorte); // Ve si puede volver al trópico

        if(sem_trywait(&renosEnPoloNorte) != 0){ // Si es el ultimo reno
            for(int i = 0; i < CANT_RENOS; i++){
                sem_post(&renosEnTropico); // Avisa a otro reno que ya pueden volver al trópico
            }
            printf("ULTIMO Reno %d va al TROPICO\n", id);
        }
        else {
            sem_post(&renosEnPoloNorte);
        }
        pthread_mutex_unlock(&mutexRenos);
    } 
} 

void *elfo(void *arg) { 
    int id = *((int*) arg);
    int time;
    
    while (1) { 
        
        printf("Elfo %d trabajando\n", id); 
        time = rand()%10 + 5;
        sleep(time);
        printf("Elfo %d tiene problemas\n", id); 

        
        pthread_mutex_lock(&mutexElfos1);
        sem_wait(&grupoDe3Elfos); // Ve si se puede unir al grupo de elfos
        sem_post(&elfosParaDespertarASanta); // Avisa que pertenece al grupo de elfos
        
        if(sem_trywait(&grupoDe3Elfos) != 0){ // Si es el 3er elfo del grupo
            sem_post(&elfosConProblemas); // Avisa a santa que lo necesitan los renos
            sem_post(&elfosOrenos); // Despierta a santa
            printf("Elfos piden ayuda a Santa\n");
        }
        else {
            sem_post(&grupoDe3Elfos); // Devuelve recurso
        }
        pthread_mutex_unlock(&mutexElfos1);
        
        sem_wait(&santaAyudaElfo); // Espera la ayuda de santa
        
        pthread_mutex_lock(&mutexElfos2);
        sem_wait(&elfosParaDespertarASanta); 
        printf("Elfo %d solucionó sus problemas con santa y vuelve a trabajar\n", id);
        
        if(sem_trywait(&elfosParaDespertarASanta) != 0){ // Si es el 3er elfo del grupo en irse
            for(int i = 0; i < CANT_GRUPO_ELFOS; i++){
                sem_post(&grupoDe3Elfos); // Avisa a otro grupo de elfos que ya pueden pedir ayuda
            }
            printf("Grupo de elfos liberado\n");
        }
        else {
            sem_post(&elfosParaDespertarASanta); 
        }
        pthread_mutex_unlock(&mutexElfos2);
    }
}


int main() { 

    pthread_t threadRenos[CANT_RENOS], threadElfos[CANT_ELFOS], threadSanta;
    
    pthread_mutex_init(&mutexRenos, NULL); 
    pthread_mutex_init(&mutexElfos1, NULL);
    pthread_mutex_init(&mutexElfos2, NULL);

    
    sem_init(&elfosOrenos, 0, 0); 
    sem_init(&renosListos, 0, 0); 
    sem_init(&elfosConProblemas, 0, 0); 
    sem_init(&renosEnPoloNorte, 0, 0);
    sem_init(&renosEnTropico, 0, CANT_RENOS);
    sem_init(&armarTrineo, 0, 0); 
    sem_init(&santaAyudaElfo, 0, 0); 
    sem_init(&grupoDe3Elfos, 0, CANT_GRUPO_ELFOS); 
    sem_init(&elfosParaDespertarASanta, 0, 0); 
    sem_init(&renosParaDespertarASanta, 0, 0);
    
    for(int i = 0; i < CANT_RENOS; i++){
        int *id = malloc(sizeof(int));
        *id = i+1;
        pthread_create(&threadRenos[i], NULL, reno, id);
    }
    for(int i = 0; i < CANT_ELFOS; i++){
        int *id = malloc(sizeof(int));
        *id = i+1;
        //pthread_create(&threadElfos[i], NULL, elfo, id);
    }
    pthread_create(&threadSanta, NULL, santa, NULL);



    for(int i = 0; i < CANT_RENOS; i++){
        pthread_join(threadRenos[i], NULL);
    }
    for(int i = 0; i < CANT_ELFOS; i++){
        pthread_join(threadElfos[i], NULL);
    }
    pthread_join(threadSanta, NULL);

    pthread_mutex_destroy(&mutexElfos1);
    pthread_mutex_destroy(&mutexElfos2);
    pthread_mutex_destroy(&mutexRenos);
    
    sem_destroy(&elfosOrenos); 
    sem_destroy(&renosListos); 
    sem_destroy(&elfosConProblemas);
    sem_destroy(&renosEnPoloNorte); 
    sem_destroy(&renosEnTropico);
    sem_destroy(&armarTrineo); 
    sem_destroy(&santaAyudaElfo); 
    sem_destroy(&grupoDe3Elfos); 
    sem_destroy(&elfosParaDespertarASanta);
    sem_destroy(&renosParaDespertarASanta);
    
    return 0;
} 
