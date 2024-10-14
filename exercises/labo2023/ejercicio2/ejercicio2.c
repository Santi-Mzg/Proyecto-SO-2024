#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define MAX_CLIENTS 3
#define CLIENT_CYCLES 10
#define NUM_OPERATIONS 4

struct Request
{
    long operation;
    long client_id;
    int N;
    int request_number;
};

struct Response
{
    long client_id;
    int value;
    long operation;
    int N;
    int request_number;
};

int SIZE_REQUEST = sizeof(struct Request) - sizeof(long);
int SIZE_RESPONSE = sizeof(struct Response) - sizeof(long);

int client_queue;
int server_queue;

int send_request(struct Request request)
{
    if (msgsnd(server_queue, &request, SIZE_REQUEST, 0) == -1)
    {
        printf("Error al enviar el mensaje\n");
        exit(EXIT_FAILURE);
    }
}

int get_response(long client_id)
{
    struct Response response;

    if (msgrcv(client_queue, &response, SIZE_RESPONSE, client_id, 0) == -1)
    {
        printf("Error al recibir el mensaje %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%ld-%d-%ld(N=%d): RESPONSE %d\n", response.client_id, response.request_number, response.operation, response.N, response.value);
    fflush(stdout);
    return response.value;
}

int client(long client_id)
{

    for (int i = 0; i < CLIENT_CYCLES; i++)
    {
        long operation = (random() % 4) + 1;
        int N = (random() % 5) + 1;
        printf("%ld-%d-%ld(N=%d): REQUEST\n", client_id, i,  operation, N);
        fflush(stdout);
        struct Request request = {
            operation,
            (long)client_id,
            N,
            i
            };

        send_request(request);
        int value = get_response(client_id);
    }

    printf("Client %d finished.\n", client_id);

    exit(EXIT_SUCCESS);
}

int create_message_queues()
{
    key_t key = ftok("/etc/hosts", 65);

    server_queue = msgget(key, 0666 | IPC_CREAT);
    if (server_queue == -1)
    {
        printf("Error al crear la cola de mensajes");
        exit(EXIT_FAILURE);
    }

    key = ftok("/etc/hosts", 66);

    client_queue = msgget(key, 0666 | IPC_CREAT);
    if (client_queue == -1)
    {
        printf("Error al crear la cola de mensajes");
        exit(EXIT_FAILURE);
    }
}

struct Request get_request(long operation)
{
    struct Request request;

    if (msgrcv(server_queue, &request, SIZE_REQUEST, operation, 0) == -1)
    {
        printf("ACA: Error al recibir el mensaje operation(%ld): %s\n", operation, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fflush(stdout);

    return request;
}

int send_response(struct Response response)
{   

    if (msgsnd(client_queue, &response, SIZE_RESPONSE, 0) == -1)
    {
        printf("Error al enviar el mensaje %s \n", strerror(errno));
        exit(1);
    }
}

int operation_1(int N)
{
    int result = 0;
    for (int i = 1; i <= N; i++)
    {
        if (i % 2 == 0)
        {
            result += i;
        }
    }

    return result;
}

int operation_2(int N)
{
    int result = 1;
    for (int i = 1; i <= N; i++)
    {
        result = result * i;
    }

    return result;
}
int operation_3(int N)
{
    int result = 0;
    for (int i = 1; i <= N; i++)
    {
        result += i * i;
    }

    return result;
}

int operation_4(int N)
{
    int result = 1;

    for (int i = 1; i <= N; i++)
    {
        if (i % 2 != 0)
        {
            result = result * i;
        }
    }

    return result;
}

int perform_operation(long operation, int N)
{

    int result;

    switch (operation)
    {
    case 1:
        result = operation_1(N);
        break;
    case 2:
        result = operation_2(N);
    case 3:
        result = operation_3(N);
        break;
    case 4:
        result = operation_4(N);
        break;
    default:
        printf("Operation %ld doesnt exists", operation);
        exit(EXIT_FAILURE);
    }

    return result;
}

void *thread_server(void *arg)
{
    long operation = *((long *) arg);
    

    while (true)
    {
        struct Request request = get_request(operation);
        struct Response response = {
            request.client_id,
            perform_operation(operation, request.N),
            request.operation,
            request.N,
            request.request_number
            };
        send_response(response);
    }
}

void init_server()
{
    pthread_t server_threads[NUM_OPERATIONS];
    printf("server\n");
    fflush(stdout);
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        long *operation = malloc(sizeof(*operation));
        *operation = i + 1;
        if (pthread_create(&server_threads[i], NULL, thread_server, (void*)operation))
        {
            printf("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }
}

int main()
{

    pid_t pid;

    create_message_queues();


    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        pid = fork();

        if (pid == 0)
        {
            long client_id = i + 1;
            client(client_id);
        }
        else if (pid < 0)
        {
            printf("Error creating child");
            exit(EXIT_FAILURE);
        }
    }


    init_server();
    // fflush(stdout);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        wait(NULL);
    }

    printf("All clients finished");

    printf("Destroy sever threads");

    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        // pthread_join(server_threads[i], NULL);
    }

    exit(EXIT_SUCCESS);
}
