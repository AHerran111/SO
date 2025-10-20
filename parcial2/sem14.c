

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NTHREADS 6

typedef struct {
     int count;
     pthread_mutex_t exmut;
     pthread_mutex_t wait;
} SEMAPHORE;

void wait(SEMAPHORE *s);
void signal(SEMAPHORE *s);
void seminit(SEMAPHORE *s,int val);
void semdestroy(SEMAPHORE *s);

SEMAPHORE sem;

void *tfunc(void *args)
{
   int i = *((int *)args);
   int n=10;

   while(n--)
   {
      wait(&sem);

      printf("Entra a la SC hilo %d -",i);
      fflush(stdout);
      usleep(rand() % 1000000);
      printf(" %d sale de la SC\n",i);

      signal(&sem);

      usleep(rand() % 1000000);
   }
}

int main()
{
   pthread_t tid[NTHREADS];
   int args[NTHREADS];
   int i;

   seminit(&sem,1);

   for(i=0;i<NTHREADS;i++)
   {
      args[i] = i;
      pthread_create(&tid[i],NULL,tfunc,&args[i]);
   }

   for(i=0;i<NTHREADS;i++)
      pthread_join(tid[i],NULL);

   semdestroy(&sem);
}

void wait(SEMAPHORE *s)
{
   pthread_mutex_lock(&s->exmut);
   s->count--;

   if(s->count < 0) {
    
      pthread_mutex_unlock(&s->exmut);
      pthread_mutex_lock(&s->wait);
   }

   else
      pthread_mutex_unlock(&s->exmut);
}

void signal(SEMAPHORE *s)
{
   pthread_mutex_lock(&s->exmut);
   s->count++;

   if(s->count <= 0)
      pthread_mutex_unlock(&s->wait);

   pthread_mutex_unlock(&s->exmut);
}

void seminit(SEMAPHORE *s,int val)
{
   pthread_mutex_init(&s->exmut,NULL);
   pthread_mutex_init(&s->wait,NULL);
   pthread_mutex_lock(&s->wait);
   s->count = val;
}

void semdestroy(SEMAPHORE *s)
{
   pthread_mutex_destroy(&s->exmut);
   pthread_mutex_destroy(&s->wait);
}

