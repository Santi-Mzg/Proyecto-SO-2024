#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define NUMBERS_OF_CLIENTS 10
#define ENTRANCE_QUEUE_SIZE 3

#define FRENCH_FRIES_DELAY 3
#define VEGAN_DELAY 5
#define SIMPLE_BURGUER_DELAY 4
#define DISPATCHER_DELAY 2

#define FRENCH_FRIES_MENU_STR "Papas Fritas"
#define VEGAN_MENU_STR "Menu Vegano"
#define SIMPLE_BURGUER_MENU_STR "Hamburguesa Simple"

#define FRENCH_FRIES_MENU_ID 1
#define VEGAN_MENU_ID 2
#define SIMPLE_BURGUER_MENU_ID 3

#define VIP_STR "VIP"
#define NO_VIP_STR "NO_VIP"

typedef struct
{
    int client_id;
    int menu_id;
    bool vip;
} ClientOrder;

int pipe_simple_burguer[2], pipe_vegan_menu[2], pipe_french_fries[2];
int pipe_entrance_queue[2], pipe_entrance_queue_vip[2];
int pipe_order_completed[NUMBERS_OF_CLIENTS][2];

/* AUX METHODS*/

const char *get_menu_str(int menu_id)
{
    switch (menu_id)
    {
    case FRENCH_FRIES_MENU_ID:
        return FRENCH_FRIES_MENU_STR;
    case VEGAN_MENU_ID:
        return VEGAN_MENU_STR;
    case SIMPLE_BURGUER_MENU_ID:
        return SIMPLE_BURGUER_MENU_STR;
    default:
        printf("Invalid menu id %d", menu_id);
        exit(1);
    }
}

const char *get_vip_str(bool vip)
{
    if (vip)
    {
        return VIP_STR;
    }
    else
    {
        return NO_VIP_STR;
    }
}

/* END AUX METHODS */

void employee(int pipe[2], const char *menu, int menu_delay)
{
    close(pipe[1]); // read_only

    while (true)
    {
        ClientOrder order;
        int read_status = read(pipe[0], &order, sizeof(order)); // Waiting for an order from dispatcher
        if (read_status != 1)
        {
            printf("Empleado(%s) PREPARA menu(%d, %s) para cliente(%d, %s)\n",
                   menu, order.menu_id, get_menu_str(order.menu_id), order.client_id, get_vip_str(order.vip));
            fflush(stdout);

            sleep(menu_delay);

            printf("Empleado(%s) FINALIZO menu(%d, %s) para cliente(%d, %s)\n",
                   menu, order.menu_id, get_menu_str(order.menu_id), order.client_id, get_vip_str(order.vip));
            fflush(stdout);
        }
    }
    exit(0);
}

void init_employee(int pipe[2], const char *menu, int menu_delay)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        printf("Creando Empleado(%s)\n", menu);
        employee(pipe, menu, menu_delay);
    }
    else if (pid < 0)
    {
        perror("Error al crear proceso hijo\n");
        exit(EXIT_FAILURE);
    }
}

void init_all_employess()
{
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

    close(pipe_simple_burguer[0]);
    close(pipe_vegan_menu[0]);
    close(pipe_french_fries[0]);
}

/* EMPLOYEE METHODS */

/* CLIENT METHODS*/
int get_clients_waiting(int pipe[2])
{
    int bytes_available;
    if (ioctl(pipe[1], FIONREAD, &bytes_available) == -1)
    {
        perror("Error trying get pipe size");
    }

    return bytes_available / sizeof(ClientOrder);
}

void client(int pipe[2], int client_id, int menu_id, bool vip)
{
    close(pipe[0]);

    ClientOrder order;
    order.client_id = client_id;
    order.menu_id = menu_id;
    order.vip = vip;

    while (true)
    {
        if (get_clients_waiting(pipe) > ENTRANCE_QUEUE_SIZE)
        {
            printf("Client(%d, %s) intenta mas tarde, mucha gente en espera\n", client_id, get_vip_str(vip));
            fflush(stdout);
            sleep(3);
        }
        else
        {
            write(pipe[1], &order, sizeof(order));

            printf("Cliente(%d, %s) ORDENA menu: %s.\n", client_id, get_vip_str(vip), get_menu_str(menu_id));
            fflush(stdout);
            exit(0);
        }
    }
}

void init_client(int client_id, int menu_id, bool vip)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        if (vip)
        {
            client(pipe_entrance_queue_vip, client_id, menu_id, vip);
            close(pipe_entrance_queue_vip[0]);
            close(pipe_entrance_queue[0]);
            close(pipe_entrance_queue[1]);
        }
        else
        {
            client(pipe_entrance_queue, client_id, menu_id, vip);
            close(pipe_entrance_queue[0]);
            close(pipe_entrance_queue_vip[0]);
            close(pipe_entrance_queue_vip[1]);
        }
    }
    else if (pid < 0)
    {
        perror("Error creating child process\n");
        exit(EXIT_FAILURE);
    }
}

void init_all_clients()
{

    pipe(pipe_entrance_queue);
    pipe(pipe_entrance_queue_vip);

    for (int i = 0; i < NUMBERS_OF_CLIENTS; i++)
    {
        int client_id = i;

        // favorecer a que salgan papas en el menu
        int menu_id = (rand() % 2);
        if (menu_id == 0)
        {
            menu_id = (rand() % 3) + 1;
        }

        bool vip = (bool)(rand() % 2);

        init_client(client_id, menu_id, vip);
    }

    close(pipe_entrance_queue[1]);
    close(pipe_entrance_queue_vip[1]);
}

/* END CLIENT METHODS*/

/* DISPATCHER */
void dispatcher()
{

    fcntl(pipe_entrance_queue[0], F_SETFL, O_NONBLOCK);
    fcntl(pipe_entrance_queue_vip[0], F_SETFL, O_NONBLOCK);

    while (true)
    {
        ClientOrder order;
        int read_status = read(pipe_entrance_queue_vip[0], &order, sizeof(order)); // intento primero obtener una order desde la cola vip
        if (read_status <= 0)
        {
            read_status = read(pipe_entrance_queue[0], &order, sizeof(order)); // sino hay order desde la cola vip entonces intento obtener de la cola de cliente comun
        }

        if (read_status > 0)
        {
            sleep(DISPATCHER_DELAY);
            printf("Empleado DESPACHA menu para Cliente(%d, %s): %s\n", order.client_id, get_vip_str(order.vip), get_menu_str(order.menu_id));
            fflush(stdout);
            switch (order.menu_id)
            {
            case FRENCH_FRIES_MENU_ID:
                write(pipe_french_fries[1], &order, sizeof(order));
                break;
            case VEGAN_MENU_ID:
                write(pipe_vegan_menu[1], &order, sizeof(order));
                break;
            case SIMPLE_BURGUER_MENU_ID:
                write(pipe_simple_burguer[1], &order, sizeof(order));
                break;
            default:
                printf("Invalid menu id %d", order.menu_id);
                exit(1);
            }
        }
        else
        {
            // no habia nadie en ninguna de las colas entonces espera y vuelve a consultar
            sleep(1);
        }
    }
}
/* END DISPATCHER */

void main()
{
    pid_t pid;

    init_all_employess();
    sleep(3);
    init_all_clients();
    sleep(1);
    dispatcher();

    for (int i = 0; i < NUMBERS_OF_CLIENTS + 4; i++)
    {
        wait(NULL);
    }
}