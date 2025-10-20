#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/wait.h>


#define NPROCS 10


// Atributos de un buzón de mensajes
#define FLAGS 0            // Valor por default que dejamos en 0
#define MAXMSG 10          // Capacidad máxima del buzón (10 mensajes)
#define MSGSIZE 128        // Tamaño máximo de cada mensaje, 128 bytes
#define CURMSGS 0          // Cantidad de mensajes en el buzón


mqd_t queue_id;	// Buzón de mensajes
struct mq_attr attr = {FLAGS,MAXMSG,MSGSIZE,CURMSGS};
int priority=1;


void proc(int i)
{
    int j;
    int n = 10;
    char mensaje[]="NADA";  // El contenido del mensaje no importa, es irrelevante


    while(n--)
    {
        // Sección de entrada
        mq_receive(queue_id,mensaje,attr.mq_msgsize,&priority); // Receive bloqueante
                                                                // Esperar a recibir el mensaje, no importa el contenido
                                                                // Para desbloquarme y entrar a la SC
        // Sección crítica
        printf("Entra a la sección crítica proceso %d",i);
        fflush(stdout);
        usleep(rand()%2000000);
        printf("-%d sale de la sección crítica\n",i);
        fflush(stdout);


        // Salida
        mq_send(queue_id,mensaje,strlen(mensaje)+1,0);          // Depositar un mensaje "vacio" en el buzón
                                                                // para que el próximo proceso pueda entrar


        // Sección restante
        usleep(rand()%1000000);
    }
    exit(0);
}




int main()
{
    int p;
    int i;
    char mensaje[] = "NADA";


    srand(getpid());
	
	// Elimina cualquier buzón ya existente 
    mq_unlink("/mqueue");
 
    // Crear buzón de mensajes con permisos de lectura y escritura
	queue_id = mq_open("/mqueue",O_RDWR | O_CREAT, 0600, &attr);
	if(queue_id==-1)
		fprintf(stderr,"Error al crear la cola de mensajes\n");


    // Poner el mensaje inicial para que al menos uno pueda entrar
    mq_send(queue_id,mensaje,attr.mq_msgsize,0);
    
    for(i=0;i<NPROCS;i++)
    {
        p = fork();
        if(p==0)
            proc(i);
    }


    for(i=0;i<NPROCS;i++)
        wait(NULL);


    // Cerrar el buzón y eliminar
    mq_close(queue_id);
    mq_unlink("/mqueue");;
}
