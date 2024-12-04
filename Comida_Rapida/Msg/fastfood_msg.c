#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/msg.h>

#define NUMBERS_OF_CLIENTS 10
#define ENTRANCE_QUEUE_SIZE 3

#define FRENCH_FRIES_DELAY 3
#define VEGAN_DELAY 5
#define SIMPLE_BURGUER_DELAY 4
#define DISPATCHER_DELAY 2
#define CLIENT_RETRY_DELAY 5

#define FRENCH_FRIES_MENU_STR "Papas Fritas"
#define VEGAN_MENU_STR "Menu Vegano"
#define SIMPLE_BURGUER_MENU_STR "Hamburguesa Simple"

#define FRENCH_FRIES_MENU_ID 1
#define VEGAN_MENU_ID 2
#define SIMPLE_BURGUER_MENU_ID 3

#define VIP_PRIORITY 1
#define NO_VIP_PRIORITY 2

#define VIP_PRIORITY_STR "VIP"
#define NO_VIP_PRIORITY_STR "NO_VIP"

struct ClientOrder
{
    long priority;
    long client_id;
    long menu_id;
};

struct DispatcherOrder
{
    long menu_id;
    long priority;
    long client_id;
};

struct OrderFinished
{
    long client_id;
    long menu_id;
    long priority;
};

int SIZE_CLIENT_ORDER = sizeof(struct ClientOrder) - sizeof(long);
int SIZE_DISPATCHER_ORDER = sizeof(struct DispatcherOrder) - sizeof(long);
int SIZE_ORDER_FINISHED = sizeof(struct OrderFinished) - sizeof(long);

int client_dispatcher_queue;
int dispatcher_employees_queue;
int order_finished_queue;

int pipe_simple_burguer[2], pipe_vegan_menu[2], pipe_french_fries[2];
int pipe_entrance_queue[2], pipe_entrance_queue_vip[2];
int pipe_order_completed[NUMBERS_OF_CLIENTS][2];

/*AUX METHODS*/
const char *get_menu_str(long menu_id)
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
        printf("Invalid menu id %ld", menu_id);
        exit(1);
    }
}

const char *get_vip_str(long priority)
{
    switch (priority)
    {
    case VIP_PRIORITY:
        return VIP_PRIORITY_STR;
    case NO_VIP_PRIORITY:
        return NO_VIP_PRIORITY_STR;
    default:
        printf("Invalid client priority %ld", priority);
        exit(1);
    }
}
/*END AUX METHODS*/

/* EMPLOYEES CODE */
struct DispatcherOrder get_dispatcher_order(long menu_id)
{
    struct DispatcherOrder dispatcher_order;

    if (msgrcv(dispatcher_employees_queue, &dispatcher_order, SIZE_DISPATCHER_ORDER, menu_id, 0) == -1)
    {
        printf("Error al recibir el mensaje %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return dispatcher_order;
}

int send_order_finished(struct OrderFinished order_finished)
{
    if (msgsnd(order_finished_queue, &order_finished, SIZE_ORDER_FINISHED, IPC_NOWAIT) == -1)
    {
        printf("Error al enviar el mensaje %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void employee(const char *emp_menu_descr, int menu_id, int menu_delay)
{
    while (true)
    {
        struct DispatcherOrder dispatcher_order = get_dispatcher_order(menu_id);
        printf("Empleado(%s) PREPARANDO menu(%ld, %s) para cliente(%ld, %s)\n",
               emp_menu_descr, dispatcher_order.menu_id, get_menu_str(dispatcher_order.menu_id),  dispatcher_order.client_id, get_vip_str(dispatcher_order.priority));
        fflush(stdout);

        sleep(menu_delay);

        struct OrderFinished order_finished = {dispatcher_order.client_id, dispatcher_order.menu_id, dispatcher_order.priority};
        send_order_finished(order_finished);
        printf("Empleado(%s) FINALIZO menu(%ld, %s) para cliente(%ld, %s)\n",
               emp_menu_descr, dispatcher_order.menu_id, get_menu_str(dispatcher_order.menu_id), dispatcher_order.client_id, get_vip_str(dispatcher_order.priority));
        fflush(stdout);
    }
    exit(0);
}

void init_employee(const char *emp_menu_descr, int menu_id, int menu_delay)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        printf("Creando Empleado(%s)\n", emp_menu_descr);
        employee(emp_menu_descr, menu_id, menu_delay);
    }
    else if (pid < 0)
    {
        perror("Error al crear proceso hijo\n");
        exit(EXIT_FAILURE);
    }
}

void init_all_employess()
{
    init_employee(SIMPLE_BURGUER_MENU_STR, SIMPLE_BURGUER_MENU_ID, SIMPLE_BURGUER_DELAY);
    init_employee(VEGAN_MENU_STR, VEGAN_MENU_ID, VEGAN_DELAY);

    for (int i = 0; i < 2; i++)
    {
        char menu_template[] = "%s %ld";
        char menu[32];
        sprintf(menu, menu_template, FRENCH_FRIES_MENU_STR, i + 1);
        init_employee(menu, FRENCH_FRIES_MENU_ID, FRENCH_FRIES_DELAY);
    }
}

/* END EMPLOYEES CODE */

/* CLIENTS CODE */
int get_clients_waiting()
{
    struct msqid_ds buf;

    if (msgctl(client_dispatcher_queue, IPC_STAT, &buf) == -1)
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    return buf.msg_qnum;
}

int send_client_order(struct ClientOrder client_order)
{
    if (msgsnd(client_dispatcher_queue, &client_order, SIZE_CLIENT_ORDER, IPC_NOWAIT) == -1)
    {
        printf("Error al enviar el mensaje %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

struct OrderFinished get_order_finished(long client_id)
{
    struct OrderFinished order_finished;

    if (msgrcv(order_finished_queue, &order_finished, SIZE_ORDER_FINISHED, client_id, 0) == -1)
    {
        printf("Error al recibir el mensaje %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return order_finished;
}

void client(long client_id, long menu_id, long priority)
{

    struct ClientOrder client_order = {priority, client_id, menu_id};

    while (get_clients_waiting() > ENTRANCE_QUEUE_SIZE)
    {
        printf("Cliente(%ld, %s) intenta mas tarde, hay mucha gente esperando.\n", client_id, get_vip_str(priority));
        fflush(stdout);
        sleep(CLIENT_RETRY_DELAY);
    }

    send_client_order(client_order);
    printf("Cliente(%ld, %s) ORDENA menu: %s.\n", client_id, get_vip_str(priority), get_menu_str(menu_id));
    fflush(stdout);

    get_order_finished(client_id);
    printf("Cliente(%ld, %s) RECIBE la orden(%s) y se va contento.\n", client_id, get_vip_str(priority), get_menu_str(menu_id));
    fflush(stdout);

    exit(0);
}

void init_client(long client_id, long menu_id, bool vip)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        if (vip)
        {
            client(client_id, menu_id, 1);
        }
        else
        {
            client(client_id, menu_id, 2);
        }
    }
    else if (pid < 0)
    {
        perror("Error al crear proceso hijo\n\n");
        exit(EXIT_FAILURE);
    }
}

void init_all_clients()
{
    for (int i = 0; i < NUMBERS_OF_CLIENTS; i++)
    {
        long client_id = i + 1;

        long menu_id = (rand() % 2);
        if (menu_id == 0)
        {
            menu_id = (rand() % 3) + 1;
        }

        bool vip = (long)(rand() % 2);

        init_client(client_id, menu_id, vip);
    }
}

/* END CLIENTS CODE */

/* DISPATCHER CODE */
int send_dispatcher_order(struct DispatcherOrder dispatcher_order)
{
    if (msgsnd(dispatcher_employees_queue, &dispatcher_order, SIZE_DISPATCHER_ORDER, IPC_NOWAIT) == -1)
    {
        printf("Error al enviar el mensaje\n");
        exit(EXIT_FAILURE);
    }
}

struct ClientOrder get_client_order()
{
    struct ClientOrder client_order;

    if (msgrcv(client_dispatcher_queue, &client_order, SIZE_CLIENT_ORDER, -2, 0) == -1)
    {
        printf("Error al recibir el mensaje %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return client_order;
}

void dispatcher()
{
    int clients_dispatched = 0;
    while (clients_dispatched < NUMBERS_OF_CLIENTS)
    {
        struct ClientOrder client_order = get_client_order();

        sleep(DISPATCHER_DELAY);

        printf("Empleado DESPACHA orden para Cliente(%ld, %s): %s\n", client_order.client_id, get_vip_str(client_order.priority), get_menu_str(client_order.menu_id));
        fflush(stdout);

        struct DispatcherOrder dispatcher_order;
        dispatcher_order.client_id = client_order.client_id;
        dispatcher_order.menu_id = client_order.menu_id;
        dispatcher_order.priority = client_order.priority;

        send_dispatcher_order(dispatcher_order);
        clients_dispatched++;
    }
}
/* END DISPATCHER CODE */

int create_message_queues()
{
    key_t key = ftok("/etc/hosts", 65);

    client_dispatcher_queue = msgget(key, 0666 | IPC_CREAT);
    if (client_dispatcher_queue == -1)
    {
        printf("Error al crear la cola de mensajes");
        exit(EXIT_FAILURE);
    }

    key = ftok("/etc/hosts", 66);

    dispatcher_employees_queue = msgget(key, 0666 | IPC_CREAT);
    if (dispatcher_employees_queue == -1)
    {
        printf("Error al crear la cola de mensajes");
        exit(EXIT_FAILURE);
    }

    key = ftok("/etc/hosts", 67);

    order_finished_queue = msgget(key, 0666 | IPC_CREAT);
    if (dispatcher_employees_queue == -1)
    {
        printf("Error al crear la cola de mensajes");
        exit(EXIT_FAILURE);
    }
}

void main()
{
    pid_t pid;

    create_message_queues();

    init_all_employess();
    sleep(3);
    init_all_clients();
    sleep(1);
    dispatcher();

    for (int i = 0; i < NUMBERS_OF_CLIENTS; i++)
    {
        wait(NULL);
    }

    printf("Todos los clientes finalizaron\n");
    exit(0);
}