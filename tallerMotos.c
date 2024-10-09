#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h>
#include <time.h>
#include <unistd.h> 

// Secuencia: ABABCABCDABABCABCD

sem_t construirNuevaMoto, rueda, chasis, motor, pinturaSinExtra, pinturaConExtra; 

void *operarioRuedas(void *arg) { 
    while (1) { 
        sem_wait(&construirNuevaMoto);
        
        sleep(1);  
        printf("Se construyó una rueda\n");
        sem_post(&rueda);
        
        sleep(1);  
        printf("Se construyó la segunda rueda\n");
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
        
        sleep(2);  
        printf("Se agregó el motor\n");
        sem_post(&motor);
    } 
} 

void *operarioPintorRojo(void *arg) { 
    while (1) { 
        if(!sem_trywait(&motor)){
            
            sleep(2);  
            printf("Se pintó la moto de ROJO\n");
            sem_post(&pinturaSinExtra);
            sem_post(&pinturaConExtra);
        }
    } 
} 

void *operarioPintorVerde(void *arg) { 
    while (1) { 
        if(!sem_trywait(&motor)){
        
            sleep(2);  
            printf("Se pintó la moto de VERDE\n");
            sem_post(&pinturaSinExtra);
            sem_post(&pinturaConExtra);
        }
    } 
} 

void *sinEquipamientoExtra(void *arg) { 
    while (1) { 
        sem_wait(&pinturaSinExtra);
        sem_wait(&pinturaSinExtra);
        
        printf("Se terminó y entregó la moto\n");
        sem_post(&construirNuevaMoto);
    } 
} 

void *operarioEquipamientoExtra(void *arg) { 
    while (1) { 
        sem_wait(&pinturaConExtra);
        sem_wait(&pinturaConExtra);
        
        sleep(2);  
        printf("Se terminó y entregó la moto con equipamiento extra\n");
        sem_post(&construirNuevaMoto);
    } 
} 

int main() { 

    pthread_t tOperarioRuedas, tOperarioChasis, tOperarioMotor, tOperarioPintorRojo, tOperarioPintorVerde, tSinEquipamientoExtra, tOperarioEquipamientoExtra;
    
    sem_init(&construirNuevaMoto, 0, 1); 
    sem_init(&rueda, 0, 0); 
    sem_init(&chasis, 0, 0); 
    sem_init(&motor, 0, 0); 
    sem_init(&pinturaSinExtra, 0, 1);  // Primero no se agrega equipamiento extra
    sem_init(&pinturaConExtra, 0, 0); 

    pthread_create(&tOperarioRuedas, NULL, operarioRuedas, NULL); 
    pthread_create(&tOperarioChasis, NULL, operarioChasis, NULL); 
    pthread_create(&tOperarioMotor, NULL, operarioMotor, NULL);
    pthread_create(&tOperarioPintorRojo, NULL, operarioPintorRojo, NULL);
    pthread_create(&tOperarioPintorVerde, NULL, operarioPintorVerde, NULL);
    pthread_create(&tSinEquipamientoExtra, NULL, sinEquipamientoExtra, NULL);
    pthread_create(&tOperarioEquipamientoExtra, NULL, operarioEquipamientoExtra, NULL);
    
    pthread_join(tOperarioRuedas, NULL); 
    pthread_join(tOperarioChasis, NULL); 
    pthread_join(tOperarioMotor, NULL);
    pthread_join(tOperarioPintorRojo, NULL);
    pthread_join(tOperarioPintorVerde, NULL);
    pthread_join(tSinEquipamientoExtra, NULL);
    pthread_join(tOperarioEquipamientoExtra, NULL);
    
    

    return 0;
} 
