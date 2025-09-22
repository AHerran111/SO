#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/time.h>

#define N_THREADS 4
#define SIZE 2000

double a[SIZE][SIZE];
double b[SIZE][SIZE];
double c[SIZE][SIZE];

void * t_func (void *args) {

    int num = *((int*)args);
    int i,j,k;

    int start = num * (SIZE/N_THREADS);
    int end = (num+1) * (SIZE/N_THREADS);

    for(i = start; i<end; i++ ) {
        for(k=0; k<SIZE; k++ )
            for(j=0; j<SIZE; j++)
                c[i][j] =  c[i][j] + a[i][k] * b[k][j];
    }
}

int main ( ) {

    pthread_t tid[N_THREADS];
    int num[N_THREADS];  
    struct timeval start,stop;

    gettimeofday(&start,NULL);

    for(int i=0; i<N_THREADS; i++){
        num[i] = i;
        pthread_create(&tid[i],NULL,t_func,&num[i]);
    }

    for(int i=0; i<N_THREADS; i++){
        pthread_join(tid[i],NULL);
    }

    gettimeofday(&stop,NULL);
    printf("Tiempo : %d\n",(int)(stop.tv_sec-start.tv_sec));

}
