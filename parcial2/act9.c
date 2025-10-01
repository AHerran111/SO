#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define NTHREADS 2
#define ITERS 5

void print(char *s)
{
    usleep(rand()%1000);
    printf("%s\n",s);
    usleep(rand()%1000);
}

void P()
{
    print("A");
    print("B");
    print("C");
    exit(0);
}

void Q()
{
    print("D");
    print("E");
    exit(0);
}

// int main()
// {
//     srand(getpid());

//     int p;

//     p=fork();
//     if(p==0)
//         P();

//     p=fork();
//     if(p==0)
//         Q();

//     wait(NULL);
//     wait(NULL);
// }



int cuenta=0;

void *total(void *args)
{
    int i;
    for(i=0;i<ITERS;i++)
       cuenta++;
}

int main()
{
    int i;
    pthread_t tid[NTHREADS];

    for(i=0;i<NTHREADS;i++)
        pthread_create(&tid[i],NULL,total,NULL);

    for(i=0;i<NTHREADS;i++)
        pthread_join(tid[i],NULL);

    printf("cuenta = %d\n",cuenta);
}
