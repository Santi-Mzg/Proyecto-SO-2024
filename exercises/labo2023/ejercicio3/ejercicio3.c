#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PEOPLE 20
#define RESTROOM_CAPACITY 3 // Capacidad del baño

// Semáforos y mutex
sem_t restroom;
sem_t female_turn;
sem_t male_turn;
pthread_mutex_t female_counter;
pthread_mutex_t male_counter;

int female_cnt = 0;
int male_cnt = 0;

void *female(void *arg);
void *male(void *arg);
void do_stuff(int id, const char *gender);

int main() {
    pthread_t threads[NUM_PEOPLE];
    sem_init(&restroom, 0, RESTROOM_CAPACITY);
    sem_init(&female_turn, 0, 1);
    sem_init(&male_turn, 0, 0);
    pthread_mutex_init(&female_counter, NULL);
    pthread_mutex_init(&male_counter, NULL);

    // Crear hilos para hombres y mujeres alternando
    for (int i = 0; i < NUM_PEOPLE; i++) {
        if (i % 2 == 0) {
            pthread_create(&threads[i], NULL, female, (void *)(long)i);
        } else {
            pthread_create(&threads[i], NULL, male, (void *)(long)i);
        }
        sleep(random()%5);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_PEOPLE; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruir semáforos y mutex
    sem_destroy(&restroom);
    pthread_mutex_destroy(&female_counter);
    pthread_mutex_destroy(&male_counter);


    return 0;
}

void *female(void *arg) {
    int id = (int)(long)arg;

    pthread_mutex_lock(&female_counter);

    female_cnt++;
    if (female_cnt == 1) {
        sem_wait(&female_turn);
        sem_trywait(&male_turn);
    }
    pthread_mutex_unlock(&female_counter);

    sem_wait(&restroom);

    do_stuff(id, "Mujer");

    sem_post(&restroom);

    pthread_mutex_lock(&female_counter);
    female_cnt--;
    if (female_cnt == 0) {
        if(male_cnt > 0){
            sem_post(&male_turn);
        }else{
            sem_post(&female_turn);
        }
    }
    pthread_mutex_unlock(&female_counter);

    return NULL;
}

void *male(void *arg) {
    int id = (int)(long)arg;

    pthread_mutex_lock(&male_counter);

    male_cnt++;
    if (male_cnt == 1) {
        sem_wait(&male_turn);
        sem_trywait(&female_turn);
    }
    pthread_mutex_unlock(&male_counter);

    sem_wait(&restroom);

    do_stuff(id, "Hombre");

    sem_post(&restroom);

    pthread_mutex_lock(&male_counter);
    male_cnt--;
    if (male_cnt == 0) {
        sem_post(&female_turn);
        sem_post(&male_turn); 
    }
    pthread_mutex_unlock(&male_counter);

    return NULL;
}

void do_stuff(int id, const char *gender) {
    printf("%s %d está usando el baño\n", gender, id);
    fflush(stdout);
    sleep(1); // Simular uso del baño
    printf("%s %d salió del baño\n", gender, id);
    fflush(stdout);
}