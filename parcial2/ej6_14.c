#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t coche_vacio;
sem_t tomar_coche;
sem_t coche_lleno;
sem_t pasaje_libre;

#define N 12
#define M 3

void *pasajero(void *args)
{
    int i = *((int *)args);
    int n=5;

    while(n--)
    {
        usleep(rand()%1000000);
        sem_wait(&coche_vacio);//hay coches?
        sem_post(&tomar_coche);
        sem_wait(&coche_lleno);

        printf("\t\t\tPasajero %d en paseo\n",i);
        sem_wait(&pasaje_libre);
        printf("\t\t\tPasajero %d termina paseo\n",i);
    }
}

void *coche(void *args)
{
    int i = *((int *)args);
    int n=20;
    while(n--)
    {
        sem_post(&coche_vacio);
        sem_wait(&tomar_coche);//hay pasajeros?
        sem_post(&coche_lleno);

        printf("Coche %d en paseo\n",i);
        usleep(rand()%1000000);
        printf("Coche %d termina\n",i);

        sem_post(&pasaje_libre);//termina viaje
    }
}

int main()
{
    int i;
    pthread_t tid_pasajero[N];
    pthread_t tid_coche[M];
    int args_pasajero[N];
    int args_coche[M];

    srand(getpid());

    sem_init(&coche_vacio,0,0);
    sem_init(&coche_lleno,0,0);
    sem_init(&tomar_coche,0,0);
    sem_init(&pasaje_libre,0,0);

    for(i=0;i<N;i++)
    {
        args_pasajero[i] = i;
        pthread_create(&tid_pasajero[i],NULL,pasajero,&args_pasajero[i]);
    }
    for(i=0;i<M;i++)
    {
        args_coche[i] = i;
        pthread_create(&tid_coche[i],NULL,coche,&args_coche[i]);
    }

    for(i=0;i<N;i++)
        pthread_join(tid_pasajero[i],NULL);
    for(i=0;i<M;i++)
        pthread_join(tid_coche[i],NULL);

    sem_destroy(&coche_lleno);
    sem_destroy(&coche_vacio);
    sem_destroy(&tomar_coche);
    sem_destroy(&pasaje_libre);
}

