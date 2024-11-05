#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h>
#include <time.h>
#include <unistd.h> 

// Secuencia: ABABCABCDABABCABCD

sem_t construirNuevaMoto, rueda, chasis, motor, pintarMoto, pintura, entregarMoto; 

void *operarioRuedas(void *arg) { 
    while (1) { 
        sem_wait(&construirNuevaMoto);
        printf("NUEVA MOTO EN CONSTRUCCIÓN\n");
        
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
        
        sleep(1);  
        printf("Se agregó el motor\n");
        sem_post(&motor);
        sem_post(&motor);
        sem_post(&pintarMoto);
    } 
} 

void *operarioPintorRojo(void *arg) { 
    while (1) { 
        sem_wait(&motor);

        if(!sem_trywait(&pintarMoto)){  // Sin lock para que halla condición de carrera
            sleep(2);  
            printf("Se pintó la moto de ROJO\n");
            sem_post(&pintura);
            sem_post(&entregarMoto);
        }
    } 
} 

void *operarioPintorVerde(void *arg) { 
    while (1) { 
        sem_wait(&motor);

        if(!sem_trywait(&pintarMoto)){  // Sin lock para que halla condición de carrera
            sleep(2);  
            printf("Se pintó la moto de VERDE\n");
            sem_post(&pintura);
            sem_post(&entregarMoto);
        }
    } 
} 

void *operarioEquipamientoExtra(void *arg) { 
    while (1) { 
        sem_wait(&pintura);
        sem_wait(&pintura);
        sleep(2);  
        printf("A esta moto SE le agregó EQUIPAMIENTO EXTRA\n");
        sem_post(&entregarMoto);
        sem_post(&entregarMoto);
    } 
} 

void *entregaDelVehiculo(void *arg) { 
    while (1) { 
        sem_wait(&entregarMoto);
        sem_wait(&entregarMoto);
        sleep(1);  
        printf("Se terminó y entregó la moto\n");
        sem_post(&construirNuevaMoto);
    } 
} 

int main() { 

    pthread_t tOperarioRuedas, tOperarioChasis, tOperarioMotor, tOperarioPintorRojo, tOperarioPintorVerde, tOperarioEquipamientoExtra, tEntregarMoto;
    
    sem_init(&construirNuevaMoto, 0, 1); 
    sem_init(&rueda, 0, 0); 
    sem_init(&chasis, 0, 0); 
    sem_init(&motor, 0, 0); 
    sem_init(&pintura, 0, 0); 
    sem_init(&pintarMoto, 0, 0);
    sem_init(&entregarMoto, 0, 1); 

    pthread_create(&tOperarioRuedas, NULL, operarioRuedas, NULL); 
    pthread_create(&tOperarioChasis, NULL, operarioChasis, NULL); 
    pthread_create(&tOperarioMotor, NULL, operarioMotor, NULL);
    pthread_create(&tOperarioPintorRojo, NULL, operarioPintorRojo, NULL);
    pthread_create(&tOperarioPintorVerde, NULL, operarioPintorVerde, NULL);
    pthread_create(&tOperarioEquipamientoExtra, NULL, operarioEquipamientoExtra, NULL);
    pthread_create(&tEntregarMoto, NULL, entregaDelVehiculo, NULL);
    
    pthread_join(tOperarioRuedas, NULL); 
    pthread_join(tOperarioChasis, NULL); 
    pthread_join(tOperarioMotor, NULL);
    pthread_join(tOperarioPintorRojo, NULL);
    pthread_join(tOperarioPintorVerde, NULL);
    pthread_join(tOperarioEquipamientoExtra, NULL);
    pthread_join(tEntregarMoto, NULL); 
    
    

    return 0;
} 
