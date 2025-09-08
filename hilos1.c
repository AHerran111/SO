#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define N_THREADS 4


void * t_func (void *args) {

    int num = *(int*)args;

    printf("hola mundo %d\n",num);
}

int main ( ) {

    int status;
    int args[N_THREADS];

    pthread_t tid[N_THREADS];

    //SOLO SE HACE UN HILO A LA VEZ, NO ES UTIL
    // for(int i=0; i<N_THREADS; i++) {
    //     pthread_create(&tid[i],NULL,t_func,&i);
    //     pthread_join(tid[i],NULL);
    // }

    //ASI SE HACEN MULITPLES THREADS A LA VEZ
     for(int i=0; i<N_THREADS; i++) {
        //NECESARIO PARA QUE EL PROGRAMA NO SE ADELANTE A LAS THREADS
        //Y HAYA i REPETIDOS/SALTEADOS
        args[i] = i; 
        pthread_create(&tid[i],NULL,t_func,&args[i]);
    }

     for(int i=0; i<N_THREADS; i++) {
        pthread_join(tid[i],NULL);
    }
}