#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

#define OUTPUT_FILE "./salida.txt"

void *task1(void *arg)
{
    FILE *file = (FILE *)arg;

    for (int i = 0; i < 30; i++)
    {
        int j = 2 + 3;
        j = 5 + j;
        j = j / 2;
        j = j * j;

        fprintf(file, "Operaciones aritméticas\n");
        fflush(file);
        sleep(1);
    }

    pthread_exit(NULL);
}

void list_dir(char *path)
{
    struct dirent *dir;
    DIR *d = opendir(path);

    if (d == NULL)
    {
        perror("Error opening directory");
        pthread_exit(NULL);
    }

    if (d)
    {
        int i = 0;

        while ((dir = readdir(d)) != NULL && i < 10)
        {
            printf("%s\n", dir->d_name);
            i++;
        }
        closedir(d);
    }

}

void *task2(void *arg)
{
    FILE *file = (FILE *)arg;

    for (int i = 0; i < 5; i++)
    {
        list_dir(".");
        fprintf(file, "Estoy mostrando el directorio actual\n");
        fflush(file);
        sleep(3);
    }

    pthread_exit(NULL);
}

void *task3(void *arg)
{
    FILE *file = (FILE *)arg;

    for (int i = 0; i < 10; i++)
    {
        sleep(2);
        printf("El resultado es %d\n", i);
        fprintf(file, "El resultado es %d\n", i);
        fflush(file);
    }

    pthread_exit(NULL);
}

int main()
{
    pid_t pid;
    FILE *file = fopen(OUTPUT_FILE, "w");

    const int NUM_THREADS = 3;
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        void *thread_function = NULL;

        switch (i)
        {
        case 0:
            thread_function = task1;
            break;
        case 1:
            thread_function = task2;
            break;
        case 2:
            thread_function = task3;
            break;
        default:
            perror("Failed assigned tasks");
            exit(EXIT_FAILURE);
            break;
        }

        if (pthread_create(&threads[i], NULL, thread_function, (void *)file))
        {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    fclose(file);

    printf("Parent (%d): Both child processes have completed.\n", getpid());

    exit(EXIT_SUCCESS);
}