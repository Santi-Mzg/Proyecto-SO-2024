#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h>
#include <time.h>
#include <unistd.h> 

#define CANT_RENOS 9
#define CANT_ELFOS 6

sem_t elfosOrenos, renosListos, elfosConProblemas, armarTrineo, santaAyudaElfo,
renosEnPoloNorte, elfosParaDespertarASanta, grupo; 

pthread_mutex_t mutexRenos, mutexElfos;

void *santa(void *arg) { 
    while (1) { 
        printf("Santa se va a dormir\n");
        sem_wait(&elfosOrenos);
        
        if(sem_trywait(&renosListos) == 0){
            printf("Se arma el trineo\n");
            sleep(5);
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo); 
            sem_post(&armarTrineo);
            printf("Santa terminó de prepara el trineo y se fue\n");
            sleep(15);
        }
        else {
            
            if(sem_trywait(&elfosConProblemas) == 0){ 
                printf("Santa ayuda a los elfos\n");
                sleep(3);
                sem_post(&santaAyudaElfo); 
                sem_post(&santaAyudaElfo); 
                sem_post(&santaAyudaElfo);
            }
        }
    } 
} 

void *reno(void *arg) { 
    int time;
    
    while (1) { 
        
        printf("Reno en el trópico\n"); 
        time = rand()%15 + 5;
        sleep(time);
        printf("Reno volvió al polo norte\n");
        
        sem_wait(&renosEnPoloNorte);
        pthread_mutex_lock(&mutexRenos);
        if(sem_trywait(&renosEnPoloNorte) != 0){ // Si es el ultimo reno
            sem_post(&elfosOrenos);
            sem_post(&renosListos); // Avisa a santa
        }
        else {
            sem_post(&renosEnPoloNorte);
        }
        pthread_mutex_unlock(&mutexRenos);

        sem_wait(&armarTrineo);
        sleep(15);
        
    } 

} 

void *elfo(void *arg) { 
    int time;
    
    while (1) { 
        
        printf("Elfo trabajando\n"); 
        time = rand()%15+5;
        sleep(time);
        printf("Elfo tiene problemas\n"); 
        
        pthread_mutex_lock(&mutexElfos);
        sem_wait(&elfosParaDespertarASanta); // De 3
        
        if(sem_trywait(&elfosParaDespertarASanta) != 0){ // Si es el 3er elfo del grupo
            sem_post(&elfosOrenos);
            sem_post(&elfosConProblemas); // Avisa a santa
            printf("Elfos piden ayuda a santa\n");
        }
        else {
            sem_post(&elfosParaDespertarASanta);
        }
        pthread_mutex_unlock(&mutexElfos);
        
        sem_wait(&santaAyudaElfo); // Espera la ayuda de santa
        sem_post(&elfosParaDespertarASanta);
        printf("Elfo solucionó sus problemas con santa y vuelve a trabajar\n");
        
    }
}


int main() { 

    pthread_t threadRenos[CANT_RENOS], threadElfos[CANT_ELFOS], threadSanta;
    
    pthread_mutex_init(&mutexRenos, NULL); 
    pthread_mutex_init(&mutexElfos, NULL);
    
    sem_init(&elfosOrenos, 0, 0); 
    sem_init(&renosListos, 0, 0); 
    sem_init(&elfosConProblemas, 0, 0); 
    sem_init(&armarTrineo, 0, 0); 
    sem_init(&santaAyudaElfo, 0, 0); 
    sem_init(&renosEnPoloNorte, 0, 9); 
    sem_init(&elfosParaDespertarASanta, 0, 3); 
    sem_init(&grupo, 0, 3); 
    
    for(int i = 0; i < CANT_RENOS; i++){
        pthread_create(&threadRenos[i], NULL, reno, NULL);
    }
    for(int i = 0; i < CANT_ELFOS; i++){
        pthread_create(&threadElfos[i], NULL, elfo, NULL);
    }
    pthread_create(&threadSanta, NULL, santa, NULL);



    for(int i = 0; i < CANT_RENOS; i++){
        pthread_join(threadRenos[i], NULL);
    }
    for(int i = 0; i < CANT_ELFOS; i++){
        pthread_join(threadElfos[i], NULL);
    }
    pthread_join(threadSanta, NULL);

    pthread_mutex_destroy(&mutexElfos);
    pthread_mutex_destroy(&mutexRenos);
    
    sem_destroy(&elfosOrenos); 
    sem_destroy(&renosListos); 
    sem_destroy(&elfosConProblemas); 
    sem_destroy(&armarTrineo); 
    sem_destroy(&santaAyudaElfo); 
    sem_destroy(&renosEnPoloNorte); 
    sem_destroy(&elfosParaDespertarASanta); 
    sem_destroy(&grupo); 
    
    return 0;
} 
