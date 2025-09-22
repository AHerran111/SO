#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

//FLAGS AND GLOBAL VALUES
int THREADS;
int TEST_FLAG = 0;
int PAR_FLAG = 1;
int MIN_THREAD = 0;
int MAX_THREAD = 1;

//RANGE OF NUMS
int start;
int end;

//POINTER TO ARRAY OF PRIME SUMS
int *count;

//PARSES ARGS WITH OPTARG()
void parse_args(int argc, char *argv[]);
//FUNC TO USE WITH pthread()
void * t_func (void *args);
//PRIME FUNCTION
int isprime(int n);
//MULTITHREAD
int par_primes();
//SEQUENTIAL
int seq_primes();

int main(int argc, char * argv[]) {
    
    //STRUCT NEEDED TO COUNT PROCCESING TIME
    struct timespec begin, finish;
    //HOLDS TOTAL OF PRIMES
    int sum = 0;

    parse_args(argc,argv);
    printf("start %d, end %d\n",start,end);

    //WHEN TESTING FLAG IS ON MIN/MAX THREAD VALUES ARE ENTERED,
    //ELSE MIN=0 MAX=1
    for(int i = MIN_THREAD; i<MAX_THREAD; i++){

        THREADS = i;
        clock_gettime(CLOCK_MONOTONIC, &begin);//CLOCK BEGINS

        if (PAR_FLAG) sum = par_primes();
        else sum = seq_primes();

        clock_gettime(CLOCK_MONOTONIC, &finish);//CLOCK ENDS
        double elapsed = (finish.tv_sec - begin.tv_sec)
                    + (finish.tv_nsec - begin.tv_nsec) / 1e9;


        printf("\nTHREADS USADAS:%d\n",THREADS);
        printf("Cuenta de primes entre %d y %d: %d\n",start,end,sum);
        printf("------------------------------\n");
        printf("Elapsed time: %.6f seconds\n\n", elapsed);

        //count is filled with calloc(), free() needed
        free(count);
        count = NULL;
    }
    
    
}


void parse_args(int argc, char *argv[]){
    
    int opt;
    int i = 0;
    char helpstring[] = {"Usage: %s [options] <start> <end>\n"
        "\n"
        "Count prime numbers in a given range, optionally using multiple threads.\n"
        "\n"
        "Options:\n"
        "  -N <num>       Set the number of threads to use (default: number of CPUs).\n"
        "  -s             Sequential mode (disable multithreading).\n"
        "  -t A..B        Testing mode: run with thread counts from A up to B-1.\n"
        "  -h             Show this help message and exit.\n"
        "\n"
        "Arguments:\n"
        "  <start>        Beginning of the integer range.\n"
        "  <end>          End of the integer range.\n"
        "\n"
        "Examples:\n"
        "  %s -N 4 1 1000000    # Count primes between 1 and 1,000,000 using 4 threads\n"
        "  %s -s 1 1000000      # Count primes sequentially\n"
        "  %s -t 1..8 1 1000000 # Run tests with 1 to 7 threads\n"
        "\n",};
    
    //FUNC PARSES ARGS FROM argv[] ON A WHILE LOOP
    //WITH PREDEFINED OPTIONS "N:hst:"
    //UNTIL THERE IS NONE, RETURNS -1
    while((opt = getopt(argc, argv, "N:hst:")) != -1) 
    { 
        i += 1;
        switch(opt) 
        { 
            case 'N':
                printf("N arg detected with value %s\n",optarg);
                THREADS = atoi(optarg);
                break;

            case 'h':
                printf(helpstring);
                exit(0);

            case 's':
                printf("Sequencial mode on,multithreads disabled\n");
                PAR_FLAG = 0;
                break;

            case 't':
                printf("TESTING ENABLED\n");
                sscanf(optarg, "%d..%d", &MIN_THREAD, &MAX_THREAD);
                TEST_FLAG = 1;
                break;

            case '?':
                printf(helpstring);
                exit(0);

        } 
    } 

    //EDGE CASES/ARG ORDER DIFFERENT AS EXPECTED
    if(!i && argc > 3) {
        printf(helpstring);
        exit(0);
    }

    start = atoi(argv[argc-2]);
    end = atoi(argv[argc-1]);

    if(start > 2147483647 || end > 2147483647){
        printf("Start or end value out of bounds\n");
        exit(0);
    }

    if (end < start) {
        int temp = start;
        start = end; end = temp;
    }
}

//CALLS isprime() ON DETERMINED RANGE SET BY
//thread_num AND THREADS DEFINED
void * t_func (void *args) {
    int nthread = *(int*)args;
    int range = (end - start);

    int first = start + (nthread * range/THREADS); 
    int last = start + ((nthread + 1) * range/THREADS);

    //NEEDED SO LAST THREAD RUN DOESNT SKIP LAST NUM
    if(nthread == THREADS - 1) last += 1;

    for(int i = first; i<last; i++) {
        if(isprime(i)) count[nthread] += 1;
        // if(!(i%(range/100))) 
        //     printf("%d%%\n",i/(range/100));
    }
    
    return NULL;
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


int seq_primes(){
    int sum = 0;

    for(int i = start; i<end; i++) {
        if(isprime(i)) sum += 1;
    }

    return sum;
}


int par_primes(){
    
    //CHECKS AMMOUNT OF THREADS POSSIBLE IN SYS IF NOT PREV DEFINED
    if (!THREADS && !TEST_FLAG) THREADS = sysconf(_SC_NPROCESSORS_ONLN);

    count = calloc(THREADS, sizeof(int));
    pthread_t tid[THREADS];//HOLDS threadId
    //HOLDS thread_num SO EACH t_func KNOWS WHO IS WHO
    int nthread[THREADS];
    int sum = 0;
    int i = 0;

    //OPENS THREADS
    for(i=0; i<THREADS; i++){
        nthread[i] = i;
        pthread_create(&tid[i],NULL,t_func,&nthread[i]);
    }

    //CLOSES THREADS
    for(i=0; i<THREADS; i++){
        pthread_join(tid[i],NULL);
    }

    //SUMS EACH TREAD PRIME COUNT
    for(i=0; i<THREADS; i++){
        sum += count[i];
    }

    
    return sum;
}