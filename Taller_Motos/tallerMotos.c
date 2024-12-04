#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h>
#include <time.h>
#include <unistd.h> 


sem_t rueda, chasis, motor, pintura, construirNuevaMoto, eqExtra; 

void *operarioRuedas(void *arg) { 
    while (1) { 
        sem_wait(&construirNuevaMoto);
        
        sleep(1);  
        printf("Se construyó una rueda\n");
        sem_post(&rueda);
    } 
} 

void *operarioChasis(void *arg) { 
    while (1) { 
        sem_wait(&rueda);
        sem_wait(&rueda);
        
        sleep(2);  
        printf("Se construyó el chasis\n");
        sem_post(&chasis);
    } 
} 

void *operarioMotor(void *arg) { 
    while (1) { 
        sem_wait(&chasis);
        
        sleep(1);  
        printf("Se agregó el motor\n");
        sem_post(&motor);
    } 
} 

void *operarioPintorRojo(void *arg) { 
    while (1) { 
        sem_wait(&motor);
        sleep(2);  
        printf("Se pintó la moto de ROJO\n");
        if(sem_trywait(&eqExtra) == 0){
            sem_post(&pintura);
        }
        else {
            sem_post(&eqExtra);
            sem_post(&construirNuevaMoto);
            sem_post(&construirNuevaMoto);
            printf("MOTO ENTREGADA\n");
            printf("NUEVA MOTO EN CONSTRUCCIÓN\n");
        }
    } 
} 

void *operarioPintorVerde(void *arg) { 
    while (1) { 
        sem_wait(&motor);
        sleep(2);  
        printf("Se pintó la moto de VERDE\n");

        if(sem_trywait(&eqExtra) == 0){ // Si se le agrega eq. extra
            sem_post(&pintura);
        }
        else { // Sino se entrega
            sem_post(&eqExtra);
            sem_post(&construirNuevaMoto);
            sem_post(&construirNuevaMoto);
            printf("MOTO ENTREGADA\n");
            printf("NUEVA MOTO EN CONSTRUCCIÓN\n");
        }
    } 
} 

void *operarioEquipamientoExtra(void *arg) { 
    while (1) { 
        sem_wait(&pintura);
        sleep(2);  
        printf("Se agrega EQUIPAMIENTO EXTRA\n");
        sem_post(&construirNuevaMoto);
        sem_post(&construirNuevaMoto);
        printf("MOTO ENTREGADA\n");
        printf("NUEVA MOTO EN CONSTRUCCIÓN\n");
    } 
} 

int main() { 

    pthread_t tOperarioRuedas, tOperarioChasis, tOperarioMotor, tOperarioPintorRojo, tOperarioPintorVerde, tOperarioEquipamientoExtra;
    
    sem_init(&rueda, 0, 0); 
    sem_init(&chasis, 0, 0); 
    sem_init(&motor, 0, 0); 
    sem_init(&pintura, 0, 0); 
    sem_init(&construirNuevaMoto, 0, 2); 
    sem_init(&eqExtra, 0, 0);

    pthread_create(&tOperarioRuedas, NULL, operarioRuedas, NULL); 
    pthread_create(&tOperarioChasis, NULL, operarioChasis, NULL); 
    pthread_create(&tOperarioMotor, NULL, operarioMotor, NULL);
    pthread_create(&tOperarioPintorRojo, NULL, operarioPintorRojo, NULL);
    pthread_create(&tOperarioPintorVerde, NULL, operarioPintorVerde, NULL);
    pthread_create(&tOperarioEquipamientoExtra, NULL, operarioEquipamientoExtra, NULL);
    
    pthread_join(tOperarioRuedas, NULL); 
    pthread_join(tOperarioChasis, NULL); 
    pthread_join(tOperarioMotor, NULL);
    pthread_join(tOperarioPintorRojo, NULL);
    pthread_join(tOperarioPintorVerde, NULL);
    pthread_join(tOperarioEquipamientoExtra, NULL);
    
    

    return 0;
} 
