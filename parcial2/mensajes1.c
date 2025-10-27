/*
 * En esta versión el receive es bloquente 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/wait.h>


// Atributos que va a tener la cola de mensajes que voy a crear
#define FLAGS 0			// Default
#define MAXMSG 6		// Máximo de mensajes en el buzón, para que el send no se bloquee
#define MSGSIZE 128		// Tamaño máximo del mensaje
#define CURMSGS 0		// Mensajes actuales en el buzón
struct mq_attr attr = {FLAGS,MAXMSG,MSGSIZE,CURMSGS};


void emisor();
void receptor();


int main()
{
	int pid;
	int status;


	mq_unlink("/mqueue");	// Eliminar cualquier cola de mensajes ya existente
	
	// Crea un proceso donde va a ejecutarse el emisor
	pid=fork();
	if(pid==0)
		emisor();	// El hijo ejecuta el emisor
	
	sleep(10);
	// Crea un proceso donde va a ejecutarse el receptor
	pid=fork();
	if(pid==0)
		receptor();	// El hijo ejecuta el receptor
	
	// Esperar a que los dos procesos terminen
	wait(&status);
	wait(&status);
	
	mq_unlink("/mqueue");	// Eliminar la cola de mensajes
}


void emisor()
{
	int i;
	mqd_t queue_id;	// Buzón de mensajes
	char mensaje[50];


	// Abrir creando buzón de mensajes con los atributos especificados previamente
	queue_id = mq_open("/mqueue",O_WRONLY | O_CREAT, 0600, &attr);
	if(queue_id==-1)
		fprintf(stderr,"Error al crear la cola de mensajes\n");






	for(i=0;i<10;i++)
	{
		// Construye un mensaje
		sprintf(mensaje,"Este es el mensaje %d",i);
		
		// Envíe el mensaje al buzón
		if(mq_send(queue_id,mensaje,attr.mq_msgsize,0)==-1)
		{
			fprintf(stderr,"Error al mandar mensaje\n");
			i--; // Si no se puede mandar el mensaje, regresa el índice el for
		}
		else
			printf("Mensaje enviado por el emisor\n");
														
		sleep(3);
	}
	
	// Enviar el mensaje "FIN"
	sprintf(mensaje,"FIN");	
	mq_send(queue_id,mensaje,strlen(mensaje)+1,10);


	mq_close(queue_id);
	exit(0);	// Termina el emisor
}




void receptor()
{
	char mensaje[50];
	int prority;


	mqd_t queue_id;	// Buzón de mensajes


	// Abrir o crear buzón de mensajes 
	// Al no poner la bandera O_NONBLOCK, es un receive bloqueante
	queue_id = mq_open("/mqueue",O_RDONLY | O_CREAT, 0600, &attr);
	if(queue_id==-1)
		fprintf(stderr,"Error al crear la cola de mensajes\n");


	do 
	{
		// Recibe un mensaje del buzón
		if(mq_receive(queue_id,mensaje,attr.mq_msgsize,&prority)==-1)
			fprintf(stderr,"\t\t\tError al recibir mensaje\n");
		else
			printf("\t\t\t%s\n",mensaje);	// Imprimir el mensaje
		usleep(500000);		// Espera medio segundo entre cada receive
	} while(strcmp(mensaje,"FIN")!=0); // Mientras no sea fin 


	mq_close(queue_id);
	exit(0);	// Termina el receptor
}
