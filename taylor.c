#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define ITERS 1000000000
#define N_THREADS 4

double sum[N_THREADS] = {0};
float x;

void * t_func (void *);

int main()
{
	long long start_ts;
	long long stop_ts;
	long long elapsed_time;
	long lElapsedTime;
	struct timeval ts;
	int n;
	double f_sum = 0.0;

    int num[N_THREADS];
    pthread_t tid[N_THREADS];
    

	printf("Calcular el logaritmo natural de 1+x\n");
	printf("Dame el valor de x :");
	scanf("%f",&x);
	
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial


    for(int i=0; i<N_THREADS; i++){
        num[i] = i;
        pthread_create(&tid[i],NULL,t_func,&num[i]);
    }

    for(int i=0; i<N_THREADS; i++){
        pthread_join(tid[i],NULL);
    }

	
    for (int i = 0; i<N_THREADS; i++ ) {
        f_sum = f_sum + sum[i];
    }

	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final

	elapsed_time = stop_ts - start_ts;
	printf("Resultado = %.8lf\n",f_sum);
	printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n",elapsed_time);
}


void * t_func (void *args) {

    int n;
    int numt = *(int *)args;
    int start = numt * (ITERS/N_THREADS) + 1;
    int end = (numt + 1) * (ITERS/N_THREADS);

    for(n=start;n<end;n++) {
		sum[numt] = sum[numt] + pow(-1,n+1) * pow(x,n) / n;

        //si el valor empieza hacer overflow(0), romper ciclo
        if(sum[numt] == 0.0) break;
    }

    //En la mayoria de casos el primer elemento dara el valor, y el resto
    //0, ya que los valores retornados son tan altos
    //que no pueden ser almacenados en double
    //printf("final[%d] = %lf\n",numt,sum[numt]);
}
