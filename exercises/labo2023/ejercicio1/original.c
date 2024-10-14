#define MAX 10
#define FIN -1

int buffer[MAX];
sem_t huecos, elementos;
int generardato(void){ return random() % 256; }
int numeroaleatorio(void){ return random() % 100; }

void *productor(void *p){
    int pos_productor = 0;
    int num, dato, n;
    n = numeroaleatorio();
    printf("Productor con %d datos \n", n);
    for(num=0; num < n; num ++){
        dato = generardato();
        sem_wait(&huecos);
        buffer[pos_productor]=dato;
        pos_productor=(pos_productor + 1) % MAX;
        sem_post(&elementos);
    }
    buffer[pos_productor] = FIN;
    pthread_exit(NULL);
}

void *consumidor(void *p){
    int pos_consumidor, dato;
    bool continuar = true;
    while(continuar){
        sem_wait(&elementos);
        dato = buffer[pos_consumidor];
        pos_consumidor = (pos_consumidor + 1) % MAX;
        if(dato == FIN)
            continuar = false;
        sem_post(&huecos);
        printf("Numeros aleatorio: %d\n", dato);
    }
    pthread_exit(NULL);
}