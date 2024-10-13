#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int load_image(char *path)
{
    printf("Child Process: Executing %s. PID: %d, Parent PID: %d\n\n", path, getpid(), getppid());
    execl(path, path, NULL);

    perror("execl failed");
    exit(EXIT_FAILURE);
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
            if (i == 0)
            {
                load_image("./counter1b.o");
            }
            else
            {
                load_image("./listdir1b.o");
            }
        }
    }

    for (int i = 0; i < 2; i++)
    {
        wait(NULL);
    }

    printf("Parent (%d): Both child processes have completed.\n", getpid());

    exit(EXIT_SUCCESS);
}