

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define TAMBUFFER 10

typedef struct {
    int in; // Posición del siguiente elemento por entrar al buffer
    int out; // Posición del siguiente elemento a retirar del buffer
    int buffer[TAMBUFFER];  // Buffer circular
} BUFFER;

void enter_monitor();
void leave_monitor();
void cwait();
void cnotify();

void buffer_init(BUFFER *bf);
void buffer_put(BUFFER *b,int data);
int buffer_get(BUFFER *b);

void *productor(void *args);
void *consumidor(void *args);;

BUFFER buffer;
int elementos=0;

sem_t monitor_enter;
sem_t monitor_blocking;

int main()
{

    sem_init(&monitor_enter,0,1);
    sem_init(&monitor_blocking,0,0);

    pthread_t tid[2];

    srand(getpid());

    buffer_init(&buffer);

    pthread_create(&tid[0],NULL,productor,NULL);
    pthread_create(&tid[1],NULL,consumidor,NULL);

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);

    sem_destroy(&monitor_blocking);
    sem_destroy(&monitor_enter);
}

void enter_monitor()
{
    sem_wait(&monitor_enter);

}

void leave_monitor()
{
    // Usa los semáforos necesarios para que 
    // el hilo libere el monitor
    sem_post(&monitor_enter);
}

void cwait()
{
    // Usa los semáforos necesarios para que el hilo 
    // que lo ejecute se bloquée en una cola de condición
    sem_wait(&monitor_blocking);

}

void cnotify()
{
    // Usa los semáforos necesarios para que el hilo 
    // que se libere un hilo que está en la cola de 
    // condición 
    sem_post(&monitor_blocking);
}

void buffer_init(BUFFER *bf) 
{
    bf->in = 0;
    bf->out = 0;
}

void buffer_put(BUFFER *bf, int e)
{
    printf("Producer enters monitor\n");
    enter_monitor();

    //printf("Producer enters monitor\n");
    while(elementos==TAMBUFFER)
        cwait();

    bf->buffer[bf->in]=e;
    bf->in++;
    bf->in=bf->in%TAMBUFFER;

    elementos++;

    cnotify();

    leave_monitor();
}

int buffer_get(BUFFER *bf)
{
    int retval;

    enter_monitor();

    while(elementos==0)
        cwait();

    retval=bf->buffer[bf->out];
    bf->out++;
    bf->out=bf->out%TAMBUFFER;

    elementos-- ;

    cnotify();

    leave_monitor();

    return(retval);
}

void *productor(void *args)
{
    int e=0;
    int n=10;
    while(n--)
    {
        e=e+1;
        printf("Productor produce %d\n",e);
        buffer_put(&buffer,e);  // Equivalente al método del monitor
        //usleep(rand()%3000000);
        sleep(1);
    }
}

void *consumidor(void *args)
{
    int e;
    int n=10;
    while(n--)
    {
        e=buffer_get(&buffer);     // Equivalente al método del monitor
        printf("\t\t\tConsumidor consume %d\n",e);
        sleep(1);
       // usleep(rand()%3000000);
    }
}

 