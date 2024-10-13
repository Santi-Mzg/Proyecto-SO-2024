#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define OUTPUT_FILE "./salida.txt"

int task1(FILE *file)
{
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

    exit(EXIT_SUCCESS);
}

int list_dir(char *path)
{
    struct dirent *dir;
    DIR *d = opendir(path);

    if (d == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
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

    return 0;
}

int task2(FILE *file)
{
    for (int i = 0; i < 5; i++)
    {
        list_dir(".");
        fprintf(file, "Estoy mostrando el directorio actual\n");
        fflush(file);
        sleep(3);
    }

    exit(EXIT_SUCCESS);
}

int task3(FILE *file)
{
    for (int i = 0; i < 10; i++)
    {
        sleep(2);
        printf("El resultado es %d\n", i);
        fprintf(file, "El resultado es %d\n", i);
        fflush(file);
    }

    exit(EXIT_SUCCESS);
}

int main()
{
    pid_t pid;
    FILE *file = fopen(OUTPUT_FILE, "w");

    for (int i = 0; i < 3; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("Error creating child");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            switch (i)
            {
            case 0:
                task1(file);
                break;
            case 1:
                task2(file);
                break;
            case 2:
                task3(file);
                break;
            default:
                break;
            }
        }
    }

    for (int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    fclose(file);

    printf("Parent (%d): Both child processes have completed.\n", getpid());
    
    exit(EXIT_SUCCESS);
}