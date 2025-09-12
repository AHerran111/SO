#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define THREADS 12
int start;
int end;
int count[THREADS] = {0};

int isprime(int n);
void * t_func (void *args);

int main(int argc, char * argv[]) {
   
    pthread_t tid[THREADS];
    int nthread[THREADS];
    int sum = 0;
    int i;
    struct timespec begin, finish;
    


    // printf("La cantidad de args:%d\n", argc);
    
    // for(int i=0; i<argc; i++) {
    //     printf("arg %d:%s\n", i, argv[i]);
    // }

    if (argc == 3) {
        start = atoi(argv[1]);
        end = atoi(argv[2]);

        printf("start %d, end %d\n",start,end);

        if (end < start) {
            int temp = start;
            start = end; end = temp;

        }
        
        clock_gettime(CLOCK_MONOTONIC, &begin);

        for(i=0; i<THREADS; i++){
            nthread[i] = i;
            pthread_create(&tid[i],NULL,t_func,&nthread[i]);
        }

        for(i=0; i<THREADS; i++){
            pthread_join(tid[i],NULL);
        }

        for(i=0; i<THREADS; i++){
            sum += count[i];
        }

        clock_gettime(CLOCK_MONOTONIC, &finish);

        double elapsed = (finish.tv_sec - begin.tv_sec)
                    + (finish.tv_nsec - begin.tv_nsec) / 1e9;

        printf("Cuenta de primes entre %d y %d: %d\n",start,end,sum);
        printf("------------------------------\n");
	    printf("Elapsed time: %.6f seconds\n", elapsed);
    }

    else
        printf("help");


}

void * t_func (void *args) {
    int nthread = *(int*)args;
    int range = (end - start);

    int first = start + (nthread * range/THREADS); 
    int last = start + ((nthread + 1) * range/THREADS);

    if(nthread ==THREADS - 1) last += 1;

    for(int i = first; i<last; i++) {
        if(isprime(i)) count[nthread] += 1;
    }
    
}


int isprime(int n)
{
	int d=3;
	int prime=n==2;
	int limit=sqrt(n);
	
	if(n>2 && n%2!=0)
	{
		while(d<=limit && n%d)
			d+=2;
		prime=d>limit;
	}
	return(prime);
}