#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>  
#include <sys/wait.h> 
#include <fcntl.h>

#define CANT_CLIENTES 50
#define TAM_COLA_ENTRADA 30


// Declaración de mutex locks
pthread_mutex_t mutex, mutexColaEntrada, mutexPedidoPapasFritasVIP, mutexPedidoPapasFritasComun;

// Declaración de pipes para la atención de los pedidos
int pedidoHamburguesa[2], pedidoHamburguesaVIP[2], entregaHamburguesaVIP[2], entregaHamburguesaComun[2], pedidoHamburguesaComun[2], 
pedidoMenuVegano[2], pedidoMenuVeganoVIP[2], entregaMenuVeganoVIP[2], entregaMenuVeganoComun[2], pedidoMenuVeganoComun[2],
pedidoPapasFritas[2], pedidoPapasFritasVIP[2], entregaPapasFritasVIP[2], entregaPapasFritasComun[2], pedidoPapasFritasComun[2];

// Declaración de pipes para las colas (cada cola con uno para espacios vacios y otro para espacios llenos)
int emptyColaEntrada[2], clientesColaEntrada[2], atencionCliente[2];

int temp_buffer;

typedef struct{
    int tipo;
    int VIP;
} * Pedido;

// ---------------------------------------Funciones-------------------------------------------------------------------




// Función del cliente
void cliente(int tipoPedido, int VIP) {
    
    // Se cierran los pipes que no se van a usar
    fcntl(emptyColaEntrada[0], F_SETFL, fcntl(emptyColaEntrada[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe emptyColaEntrada sea no bloqueante.
   
    Pedido pedido = (Pedido) malloc(sizeof(*pedido));
    pedido->tipo = tipoPedido;
    pedido->VIP = VIP;
   
    sleep(1);
    
    pthread_mutex_lock(&mutexColaEntrada);
    if(read(emptyColaEntrada[0], &temp_buffer, sizeof(temp_buffer)) > 0) {

        pthread_mutex_unlock(&mutexColaEntrada);
        pthread_mutex_lock(&mutex);
        write(clientesColaEntrada[1], pedido, sizeof(Pedido));
        read(atencionCliente[0], &temp_buffer, sizeof(temp_buffer));
        pthread_mutex_unlock(&mutex);
        
        printf("Cliente %d con pedido %d se suma a la cola de entrada.\n", pedido->VIP, pedido->tipo);
        
        if(pedido->tipo == 1) { // Hamburguesa 
            if(pedido->VIP) {
                read(entregaHamburguesaVIP[0], &temp_buffer, sizeof(temp_buffer)); // Espera su pedido
                printf("Un cliente VIP retiró una Hamburguesa\n");
            }
            else {
                read(entregaHamburguesaComun[0], &temp_buffer, sizeof(temp_buffer)); // Espera su pedido
                printf("Un cliente Comun retiró una Hamburguesa\n");
            }
        }
        else if(pedido->tipo == 2) { // Menu Vegano 
            if(pedido->VIP) {
                read(entregaMenuVeganoVIP[0], &temp_buffer, sizeof(temp_buffer)); // Espera su pedido
                printf("Un cliente VIP retiró un Menú Vegano\n");
            }
            else {
                read(entregaMenuVeganoComun[0], &temp_buffer, sizeof(temp_buffer)); // Espera su pedido
                printf("Un cliente Comun retiró un Menú Vegano\n");
            }
        }
        else if(pedido->tipo == 3) { // Papas Fritas
            if(pedido->VIP) {
                read(entregaPapasFritasVIP[0], &temp_buffer, sizeof(temp_buffer)); // Espera su pedido  
                printf("Un cliente VIP retiró un Menú Vegano\n");
            }
            else {
                read(entregaPapasFritasComun[0], &temp_buffer, sizeof(temp_buffer)); // Espera su pedido
                printf("Un cliente Comun retiró unas Papas Fritas\n");
            }
        }
        
    }
    else {
        pthread_mutex_unlock(&mutexColaEntrada);
        sleep(1);
        printf("Cliente se retira. La cola de entrada está llena.\n");
    }
}


void empleadoCajero() {
    
    Pedido pedido;

    // Se cierran los pipes que no se van a usar
    
    while(1) {
        
        read(clientesColaEntrada[0], pedido, sizeof(Pedido));
        write(atencionCliente[1], &temp_buffer, sizeof(temp_buffer));
        write(emptyColaEntrada[1], &temp_buffer, sizeof(temp_buffer));
        
        if(pedido->tipo == 1) { // Hamburguesa 
        
            write(pedidoHamburguesa[1], &temp_buffer, sizeof(temp_buffer));
        
            if(pedido->VIP) {
                write(pedidoHamburguesaVIP[1], &temp_buffer, sizeof(temp_buffer));
                printf("Un cliente VIP pidió una Hamburguesa\n");
            }
            else {
                write(pedidoHamburguesaComun[1], &temp_buffer, sizeof(temp_buffer));
                printf("Un cliente Comun pidió una Hamburguesa\n");
            }
        }
        else if(pedido->tipo == 2) { // Menu Vegano
        
            write(pedidoMenuVegano[1], &temp_buffer, sizeof(temp_buffer));
        
            if(pedido->VIP) {
                write(pedidoMenuVeganoVIP[1], &temp_buffer, sizeof(temp_buffer));
                printf("Un cliente VIP pidió un Menú Vegano\n");
            }
            else {
                write(pedidoMenuVeganoComun[1], &temp_buffer, sizeof(temp_buffer));
                printf("Un cliente Comun pidió un Menú Vegano\n");
            }
        }
        else if(pedido->tipo == 3) { // Papas Fritas
        
            write(pedidoPapasFritas[1], &temp_buffer, sizeof(temp_buffer));
        
            if(pedido->VIP) {
                write(pedidoPapasFritasVIP[1], &temp_buffer, sizeof(temp_buffer));
                printf("Un cliente VIP pidió un Papas Fritas\n");
            }
            else {
                write(pedidoPapasFritasComun[1], &temp_buffer, sizeof(temp_buffer));
                printf("Un cliente Comun pidió unas Papas Fritas\n");
            }
        }
    }
}
    
// Función del empleado que prepara las hamburguesas simples
void empleadoHamburguesas() {
    
    fcntl(pedidoHamburguesaVIP[0], F_SETFL, fcntl(pedidoHamburguesaVIP[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe clientesColaPoliticos sea no bloqueante.
    fcntl(pedidoHamburguesaComun[0], F_SETFL, fcntl(pedidoHamburguesaComun[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe clientesColaComunes sea no bloqueante.
    
    while(1) {
        read(pedidoHamburguesa[0], &temp_buffer, sizeof(temp_buffer)); // Busca si hay alguien para atender sino se va a dormir
        
		if(read(pedidoHamburguesaVIP[0], &temp_buffer, sizeof(temp_buffer)) > 0) { // Si hay un politico en la cola correspondiente para atender

		    printf("Una Hamburguesa VIP está siendo preparada\n");
		    sleep(3);
		    
		    write(entregaHamburguesaVIP[1], &temp_buffer, sizeof(temp_buffer)); // Le da atención al cliente
		}
		else if(read(pedidoHamburguesaComun[0], &temp_buffer, sizeof(temp_buffer)) > 0) { // Sino busca si hay un cliente común en la cola correspondiente para atender
		    
    		printf("Una Hamburguesa Comun está siendo preparada\n");
    		sleep(3);
    		
		    write(entregaHamburguesaComun[1], &temp_buffer, sizeof(temp_buffer)); // Le da atención al cliente
		}
    }
}

// Función del empleado que prepara las hamburguesas simples
void empleadoMenuVegano() {
    
    fcntl(pedidoMenuVeganoVIP[0], F_SETFL, fcntl(pedidoMenuVeganoVIP[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe clientesColaPoliticos sea no bloqueante.
    fcntl(pedidoMenuVeganoComun[0], F_SETFL, fcntl(pedidoMenuVeganoComun[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe clientesColaComunes sea no bloqueante.
    
    
    while(1) {
        read(pedidoMenuVegano[0], &temp_buffer, sizeof(temp_buffer)); // Busca si hay alguien para atender sino se va a dormir
        
		if(read(pedidoMenuVeganoVIP[0], &temp_buffer, sizeof(temp_buffer)) > 0) { // Si hay un politico en la cola correspondiente para atender
	    
		    printf("Un Menú Vegano VIP está siendo preparado\n");
		    sleep(3);
		    
		    write(entregaMenuVeganoVIP[1], &temp_buffer, sizeof(temp_buffer)); // Le da atención al cliente
		}
		else if(read(pedidoMenuVeganoComun[0], &temp_buffer, sizeof(temp_buffer)) > 0) { // Sino busca si hay un cliente común en la cola correspondiente para atender
		    
    		printf("Un Menú Vegano Comun está siendo preparado\n");
    		sleep(3);
    		
		    write(entregaMenuVeganoComun[1], &temp_buffer, sizeof(temp_buffer)); // Le da atención al cliente
		}
    }
}

void empleadoPapasFritas() {
    
    fcntl(pedidoPapasFritasVIP[0], F_SETFL, fcntl(pedidoPapasFritasVIP[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe clientesColaPoliticos sea no bloqueante.
    fcntl(pedidoPapasFritasComun[0], F_SETFL, fcntl(pedidoPapasFritasComun[0], F_GETFL) | O_NONBLOCK); // Hace que la lectura del pipe clientesColaComunes sea no bloqueante.
    
    while(1) {
        read(pedidoPapasFritas[0], &temp_buffer, sizeof(temp_buffer)); // Busca si hay alguien para atender sino se va a dormir
        
        pthread_mutex_lock(&mutexPedidoPapasFritasVIP);
		if(read(pedidoPapasFritasVIP[0], &temp_buffer, sizeof(temp_buffer)) > 0) { // Si hay un politico en la cola correspondiente para atender
	        pthread_mutex_unlock(&mutexPedidoPapasFritasVIP);
	        
		    printf("Unas Papas Fritas VIP están siendo preparadas\n");
		    sleep(3);
		    
		    write(entregaPapasFritasVIP[1], &temp_buffer, sizeof(temp_buffer)); // Le da atención al cliente
		}
		else {
		    pthread_mutex_unlock(&mutexPedidoPapasFritasVIP);
		    
            pthread_mutex_lock(&mutexPedidoPapasFritasComun);
		    if(read(pedidoPapasFritasComun[0], &temp_buffer, sizeof(temp_buffer)) > 0) { // Sino busca si hay un cliente común en la cola correspondiente para atender
		        pthread_mutex_unlock(&mutexPedidoPapasFritasVIP);
		        
        		printf("Unas Papas Fritas VIP están siendo preparadas\n");
        		sleep(3);
        		
    		    write(entregaPapasFritasComun[1], &temp_buffer, sizeof(temp_buffer)); // Le da atención al cliente
		    }
		    else {
		        pthread_mutex_unlock(&mutexPedidoPapasFritasComun);
		    }
		}
    }
}

int main() { 
    // Declaración de pid
    pid_t pidClientes[CANT_CLIENTES], pidEmpleadoCajero, pidEmpleadoHamburguesas, pidEmpleadoMenuVegano, pidEmpleadosPapasFritas[2]; 

    // Inicialización de mutex locks.
    pthread_mutex_init(&mutex, NULL); 
    pthread_mutex_init(&mutexColaEntrada, NULL); 
    pthread_mutex_init(&mutexPedidoPapasFritasComun, NULL); 
    pthread_mutex_init(&mutexPedidoPapasFritasVIP, NULL);
    
    // Creacion de pipes  
    if (pipe(emptyColaEntrada) == -1 || pipe(clientesColaEntrada) == -1 || pipe(pedidoHamburguesa) == -1 || pipe(pedidoHamburguesaVIP) == -1 || 
    pipe(pedidoHamburguesaComun) == -1 || pipe(pedidoMenuVegano) == -1 || pipe(pedidoMenuVeganoVIP) == -1 || pipe(pedidoMenuVeganoComun) == -1 || 
    pipe(pedidoPapasFritas) == -1 || pipe(pedidoPapasFritasVIP) == -1 || pipe(pedidoPapasFritasComun) == -1 || pipe(entregaHamburguesaVIP) == -1 ||
    pipe(entregaPapasFritasComun) == -1 || pipe(entregaHamburguesaComun) == -1 || pipe(entregaPapasFritasVIP) == -1 || pipe(entregaMenuVeganoComun) == -1 || 
    pipe(entregaMenuVeganoVIP) == -1 || pipe(atencionCliente) == -1){  
        perror("Error creando los pipes");  
        exit(1);  
    }
    
    // Inicialización de escrituras en los pipes.
    for(int i=0; i<TAM_COLA_ENTRADA; i++) {
        write(emptyColaEntrada[1], &temp_buffer, sizeof(temp_buffer));
    }

    // Creación de procesos.
    pidEmpleadoCajero = fork(); // Empleado hamburguesas
    if (pidEmpleadoCajero < 0) {
        perror("Error fork empleado cajero");
        exit(EXIT_FAILURE);
    }
    else if (pidEmpleadoCajero == 0) { // Hijo
        empleadoCajero();
        exit(EXIT_SUCCESS);
    }
    
    pidEmpleadoHamburguesas = fork(); // Empleado hamburguesas
    if (pidEmpleadoHamburguesas < 0) {
        perror("Error fork empleado hamburguesas");
        exit(EXIT_FAILURE);
    }
    else if (pidEmpleadoHamburguesas == 0) { // Hijo
        empleadoHamburguesas();
        exit(EXIT_SUCCESS);
    }
    
    pidEmpleadoMenuVegano = fork();
    if (pidEmpleadoMenuVegano < 0) {
        perror("Error fork empleado menu vegano");
        exit(EXIT_FAILURE);
    }
    else if (pidEmpleadoMenuVegano == 0) { // Hijo
        empleadoMenuVegano();
        exit(EXIT_SUCCESS);
    }
 
    for(int i = 0; i < 2; i++) { // Empleados Papas Fritas
        pidEmpleadosPapasFritas[i] = fork();
        if (pidEmpleadosPapasFritas[i] < 0) {  
            perror("Error fork empleado papas fritas");  
            exit(EXIT_FAILURE);  
        }
        else if (pidEmpleadosPapasFritas[i] == 0) { // Hijo
            empleadoPapasFritas();
            exit(EXIT_SUCCESS);
        } 
    }
    
    for(int i = 0; i < 5; i++) { // Clientes
        //sleep(1); // Para que entren de a uno
        pidClientes[i] = fork();
        
        if (pidClientes[i] < 0) {  
            perror("Error fork cliente");  
            exit(EXIT_FAILURE);  
        }
        else if (pidClientes[i] == 0) { // Hijo
            
            srand(i);
            int tipoPedido = rand() % 3 + 1;
            int VIP = rand() % 2;
            cliente(tipoPedido, VIP);
            exit(EXIT_SUCCESS);
        } 
    }
    
    // Espera de procesos
    for(int i=0; i<CANT_CLIENTES + 4; i++) { // Aqui se queda bloqueado el hilo principal esperando al hilo de empleado por lo que no se ejecuta el codigo restante
        wait(NULL);
    } 
    
    // Finalización de mutex locks
    
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexColaEntrada);
    pthread_mutex_destroy(&mutexPedidoPapasFritasVIP);
    pthread_mutex_destroy(&mutexPedidoPapasFritasComun);
    
    return 0; 
}
