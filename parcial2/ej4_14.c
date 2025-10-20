
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Constantes
#define K 2
#define NTHREADS 4

// Variables
int contador;
int bloqueados=0;

pthread_mutex_t mutex;
pthread_mutex_t check;
pthread_mutex_t retardo;

void *tfunc(void *args)
{
    int i=*((int *) args);
    int n=5;
    while(n--)
    {
        pthread_mutex_lock(&mutex);
        if(contador==0)
        {
            bloqueados++;
            printf("%d entra a bloqueados\n", i);
            pthread_mutex_unlock(&mutex);
            pthread_mutex_lock(&retardo);
            printf("%d sale de bloqueados\n", i);
        }
        else
            contador--;
            
        pthread_mutex_unlock(&mutex);

        // Sección crítica
        printf("Entra a la SC %d\n",i);
        fflush(stdout);
        usleep(rand()%500000);
        printf("%d sale de la SC\n",i);

        pthread_mutex_lock(&check);
        if(bloqueados>0)
        {
            pthread_mutex_unlock(&retardo);
            bloqueados--;        
        }
        else
            contador++;
        pthread_mutex_unlock(&check);

        // Sección restante
        usleep(rand()%500000);
    }
}

int main()
{
    int i;
    pthread_t tid[NTHREADS];
    int args[NTHREADS];

    contador=K;
    srand(getpid());

    pthread_mutex_init(&mutex,NULL); // mutex = 1;
    pthread_mutex_init(&retardo,NULL);
    pthread_mutex_init(&check,NULL);

    pthread_mutex_lock(&retardo); // retardo = 0

    for(i=0;i<NTHREADS;i++)
    {
        args[i] = i;
        pthread_create(&tid[i],NULL,tfunc,&args[i]);
    }

    for(i=0;i<NTHREADS;i++)
        pthread_join(tid[i],NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&retardo);
    pthread_mutex_destroy(&check);
}

