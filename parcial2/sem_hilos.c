#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>


#define NTHREADS 10


sem_t s1;   // La variable de tipo semáforo, debe ser global
            // Tiene que ser visible para todos los hilos

void random_wait()
{
    usleep(1000000 + rand() % 1000000);
}




void *thread(void *args)
{
    int i = *((int *) args);
    int j;
    int n = NTHREADS;


    while(n--)
    {
        // Sección de entrada
        sem_wait(&s1);


        // Sección crítica
        printf("Hilo %d entra a la sección crítica",i);
        fflush(stdout);
        random_wait();
        printf("-Hilo %d sale de la sección crítica\n",i);
        fflush(stdout);


        // Salida
        sem_post(&s1);


        // Sección restante
        random_wait();
    }
}




int main()
{
    pthread_t tid[NTHREADS];
    int targs[NTHREADS];
    int i;


    srand(getpid());
	
    // Semáforos no nombrados
	// Iniciliza datos semáforos
    sem_init(&s1,0,1);
		
    
    for(i=0;i<NTHREADS;i++)
    {
        targs[i] = i;
        pthread_create(&tid[i],NULL,thread,&targs[i]);
    }


    for(i=0;i<NTHREADS;i++)
        pthread_join(tid[i],NULL);


    sem_destroy(&s1);
}
