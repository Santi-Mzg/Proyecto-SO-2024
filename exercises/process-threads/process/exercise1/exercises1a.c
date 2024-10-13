#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

int task1()
{
    struct dirent *dir;
    DIR *d = opendir("/etc");

    if (d == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    if (d) {
        int i = 0;

        while ((dir = readdir(d)) != NULL && i < 10) {
            sleep(1);
            printf("%s\n", dir->d_name);
            i++;
        }
        closedir(d);
    }

    return(0);
}

int task2()
{
    for(int i = 0; i<10; i++){
        sleep(2);
        printf("%d\n", i + 1);
    }

}

int main()
{
    pid_t pid;

    for (int i = 0; i < 2; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("Error creating child");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            printf("Child %d: Performing task %d. PID: %d, Parent PID: %d\n", i + 1, i + 1, getpid(), getppid());
            if (i == 0)
            {
                task1();
            }
            else
            {
                task2();
            }
            printf("Child %d: Task %d completed.\n", i + 1, i + 1);
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < 2; i++){
        wait(NULL);
    }

    printf("Parent (%d): Both child processes have completed.\n", getpid());
     
    exit(EXIT_SUCCESS);
}