#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>


#define TAMBUFFER 8

#define TESPERAPROD 1000000	// Microsegundos
#define TESPERACONS 1000000


#define LIMITE 100
#define FIN 0


void productor();
void consumidor();


int isprime(int n);


typedef struct {
	int in;	// Donde va a estar el siguiente elemento que voy a meter al buffer
	int out;	// Donde está el siguiente elemento que voy a sacar del buffer
	int buffer[TAMBUFFER];	// Buffer circular
} BUFFER;


void buffer_put(BUFFER *b,int data);
int buffer_get(BUFFER *b);


BUFFER *bf;


sem_t *e_max;	// Bloquea al productor, inicia en TAMBUFFER
sem_t *n_blok;	// Bloquea al consumidor, inicia en 0
sem_t *s_exmut;


int main()
{
    int res;
    int n;
 	int p;
	int shmid;


	srand(getpid());


	// Eliminar los semáforos si ya están
    sem_unlink("/e_max");
    sem_unlink("/n_blok");
    sem_unlink("/s_exmut");


    // Creación del arreglo de semáforos
	e_max = sem_open("/e_max",O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,TAMBUFFER); // Inicia con el tamaño del buffer
	n_blok = sem_open("/n_blok",O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,0);	// Inicia en 0
	s_exmut = sem_open("/s_exmut",O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,1); // Inicia en 1


    printf("Semáforos creados\n");


	// Crear la memoria compartida
	shmid=shmget((key_t) 0x6812,sizeof(BUFFER),0666|IPC_CREAT);
	bf=shmat(shmid,NULL,0);	// Conectar y obtenemos la dirección donde está la memoria compartida
	
    /* Aquí se crean los procesos */
	p=fork();
	if(p==0)
		productor();


	p=fork();
	if(p==0)
		consumidor();

	sleep(0);



  
    for(n=0;n<2;n++)
  		wait(NULL);




    // Borrar los semáforos
	sem_close(e_max);
    sem_unlink("/e_max");


	sem_close(n_blok);
    sem_unlink("/n_blok");


	sem_close(s_exmut);
    sem_unlink("/s_exmut");




	shmdt(bf);	// Desconectar la memoria compartida al terminar
	shmctl(shmid,IPC_RMID,NULL);	// Pedir al SO que elimine la memoria compartida


    exit(EXIT_SUCCESS);
}




void productor()
{
    int n;


    printf("Inicia productor\n");
    for(n=1;n<=LIMITE;n++)
    {
		if(isprime(n) || n==LIMITE)
		{
			sem_wait(e_max);	// Si se llena el buffer se bloquea
	      	sem_wait(s_exmut);	// Asegurar el buffer como sección crítoca
			
			if(n!=LIMITE)
			{
        		buffer_put(bf,n);				// Poner el número primo encontrado en el buffer
				printf("Productor produce %d\n",n);
			}
			else
				buffer_put(bf,FIN);			// Si llegué al límite voy a poner 0 en el buffer
										// El 0 significa FIN o terminado
        	
		
        	sem_post(s_exmut);	// Libera la sección crítica del buffer
			sem_post(n_blok);	// Si el consumidor está bloqueado porque el buffer está vacío, lo desbloqueas

		}
    }
    exit(0);
}




void consumidor()
{
    int n=1;


    printf("Inicia Consumidor\n");
    while(n)
    {
        sem_wait(n_blok);	// Si el buffer está vacío, se bloquea
        sem_wait(s_exmut);	// Asegura el buffer como sección crítica 


		n=buffer_get(bf);
		if(n!=FIN)
        	printf("\nConsumidor consume %d\n",n);	
        


        sem_post(s_exmut);	// Libera la SC el buffer
        sem_post(e_max);	// Si el productor está bloqueado porque el buffer estaba lleno, lo desbloquea


    }
    exit(0);
}
     
int isprime(int n)
{
	int d=3;
	int prime=0;
	int limit=sqrt(n);
	
	prime = n == 2;


	if(n>2 && n%2)
	{
		while(d<=limit && n%d)
			d+=2;
		prime = d > limit;
	}


	return(prime);
}


void buffer_put(BUFFER *b,int data)
{
	b->buffer[b->in] = data;
	b->in = (b->in + 1) % TAMBUFFER;
}


int buffer_get(BUFFER *b)
{
	int retval = b->buffer[b->out];
	b->out = (b->out + 1) % TAMBUFFER;
	return retval;
}
