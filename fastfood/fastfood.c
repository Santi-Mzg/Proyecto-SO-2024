#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBERS_OF_CLIENTS 50
#define ENTRANCE_QUEUE_SIZE 30

#define FRENCH_FRIES_DELAY 3
#define VEGAN_DELAY 5
#define SIMPLE_BURGUER_DELAY 4

#define FRENCH_FRIES_MENU_STR "French Fries"
#define VEGAN_MENU_STR "Vegan Menu"
#define SIMPLE_BURGUER_MENU_STR "Simple Burguer"

int pipe_simple_burguer[2], pipe_vegan_menu[2], pipe_french_fries[2];

void employee(int pipe[2], const char *menu, int menu_delay)
{
    close(pipe[1]); // read_only

    while (1)
    {
        int client_id;
        int read_status = read(pipe[0], &client_id, sizeof(int)); // Waiting for an order from
        if (read_status != 1)
        {
            printf("Employee(%s) PREPARING menu to client(%d)\n", menu, client_id);
            sleep(menu_delay);
            printf("Employee(%s) FINISHED menu to client(%d)\n", menu, client_id);
        }
    }
    exit(0);
}

void init_employee(int pipe[2], const char *menu, int menu_delay)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        printf("Creating Employee(%s)\n", menu);
        employee(pipe, menu, menu_delay);
    }
    else if (pid < 0)
    {
        perror("Error creating child process\n");
        exit(EXIT_FAILURE);
    }
}

void main()
{
    pid_t pid;

    pipe(pipe_simple_burguer);
    pipe(pipe_vegan_menu);
    pipe(pipe_french_fries);

    init_employee(pipe_simple_burguer, SIMPLE_BURGUER_MENU_STR, SIMPLE_BURGUER_DELAY);
    init_employee(pipe_vegan_menu, VEGAN_MENU_STR, VEGAN_DELAY);

    for (int i = 0; i < 2; i++)
    {
        char menu_template[] = "%s %d";
        char menu[32];
        sprintf(menu, menu_template, FRENCH_FRIES_MENU_STR, i + 1);
        init_employee(pipe_french_fries, menu, FRENCH_FRIES_DELAY);
    }

    //close pipeline to other employee
    close(pipe_simple_burguer[0]);
    close(pipe_vegan_menu[0]);
    close(pipe_french_fries[0]);
    
    int client_id = 1;
    write(pipe_simple_burguer[1], &client_id, sizeof(int));

    for (int i = 0; i < 4; i++)
    {
        wait(NULL);
    }
}