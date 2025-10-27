/*
 * En esta versión el send y receive son no bloquentes 
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
#define MAXMSG 6		// Máximo de mensajes en el buzón
#define MSGSIZE 128		// Tamaño máximo del mensaje
#define CURMSGS 0		// Mensajes actuales en el buzón
struct mq_attr attr = {FLAGS,MAXMSG,MSGSIZE,CURMSGS};


void emisor();
void receptor();


int main()
{
	int pid;
	int status;




	mq_unlink("/mqueue");


	
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
	
	mq_unlink("/mqueue");
}


void emisor()
{
	int i;
	mqd_t queue_id;	// Buzón de mensajes
	char mensaje[50];


	// Crear buzón de mensajes 
	queue_id = mq_open("/mqueue",O_WRONLY | O_CREAT | O_NONBLOCK, 0600, &attr);
	if(queue_id==-1)
		fprintf(stderr,"Error al crear la cola de mensajes\n");




;
	for(i=0;i<10;i++)
	{
		// Construye un mensaje
		sprintf(mensaje,"Este es el mensaje %d",i);
		
		// Envíe el mensaje al buzón
		if(mq_send(queue_id,mensaje,attr.mq_msgsize,0)==-1)
		{
			fprintf(stderr,"No hay espacio en el buzón, error al mandar mensaje\n");
			i--;  //Si no se manda el mensaje, regresamos el contador.
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


	// Crear buzón de mensajes 
	// Si quiero que los receives sean NO BLOQUEANTES, necesito poner la bandera
	// O_NONBLOCK
	queue_id = mq_open("/mqueue",O_RDONLY | O_CREAT | O_NONBLOCK, 0600, &attr);
	if(queue_id==-1)
		fprintf(stderr,"Error al crear la cola de mensajes\n");


	do 
	{
		// Recibe un mensaje del buzón
		if(mq_receive(queue_id,mensaje,attr.mq_msgsize,&prority)==-1)
			fprintf(stderr,"\t\t\tNo hay mensaje en el buzón\n");
		else
			printf("\t\t\t%s\n",mensaje);	// Imprimir el mensaje
		usleep(500000);	// .5 segundos
	} while(strcmp(mensaje,"FIN")!=0); // Mientras no sea fin 


	mq_close(queue_id);


	exit(0);	// Termina el receptor
}
