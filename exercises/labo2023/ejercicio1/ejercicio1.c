#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX 8
#define FIN -1

int buffer[MAX];
sem_t huecos, elementos;
pthread_mutex_t mutex;
int pos_productor = 0;


int generardato(void) { return random() % 256; }
int numeroaleatorio(void) { return random() % 10; }

void producir(int dato){
    sem_wait(&huecos);

    pthread_mutex_lock(&mutex);
    sleep(2);
    buffer[pos_productor] = dato;
    pos_productor = (pos_productor + 1) % MAX;
    pthread_mutex_unlock(&mutex);

    sem_post(&elementos);
}

void *productor(void *p)
{
    int num, dato, n;

    n = numeroaleatorio();
    printf("Productor con %d datos \n", n);

    for (num = 0; num < n; num++)
    {
        dato = generardato();
        producir(dato);
    }

    pthread_exit(NULL);
}

void *consumidor(void *p)
{
    int pos_consumidor, dato;
    bool continuar = true;
    pos_consumidor = 0;

    while (continuar)
    {
        sem_wait(&elementos);
 
        pthread_mutex_lock(&mutex);

        dato = buffer[pos_consumidor];
        pos_consumidor = (pos_consumidor + 1) % MAX;
        if (dato == FIN)
            continuar = false;
        sleep(2);
        pthread_mutex_unlock(&mutex);

        sem_post(&huecos);
        printf("Numeros aleatorio: %d\n", dato);
    }

    pthread_exit(NULL);
}

int main()
{
    const int PRODUCTOR_MAX = 3;
    pthread_t consumidor_t, productor_t[PRODUCTOR_MAX];

    sem_init(&huecos, 0, MAX);
    sem_init(&elementos, 0, 0);

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        perror("Fail init mutex");
        return 1;
    }

    for (int i = 0; i < PRODUCTOR_MAX; i++)
    {
        if (pthread_create(&(productor_t[i]), NULL, productor, NULL) != 0)
        {
            perror("Error creating productor thread for listing directory");
            exit(EXIT_FAILURE);
        }
    }

    if (pthread_create(&consumidor_t, NULL, consumidor, NULL) != 0)
    {
        perror("Error creating consumidor thread for counting");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < PRODUCTOR_MAX; i++)
    {
        pthread_join(productor_t[i], NULL);
    }

    producir(FIN);

    pthread_join(consumidor_t, NULL);

    printf("Main: Both threads have completed.\n\n");

    return 0;
}