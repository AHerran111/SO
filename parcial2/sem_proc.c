#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/stat.h>

#define NPROCS 10


sem_t *s1;  // Defino la variable semáforo como un apuntador a un sem_t


void random_wait()
{
    usleep(200000 + rand() % 200000);
}


void proc(int i)
{
    int j;
    int n = 10;


    while(n--)
    {
        // Sección de entrada
        sem_wait(s1);


        // Sección crítica
        printf("Proceso %d entra a la sección crítica",i);
        fflush(stdout);
        random_wait();
        printf("-Proceso %d sale de la sección crítica\n",i);
        fflush(stdout);


        // Salida
        sem_post(s1);


        // Sección restante
        random_wait();
    }
    exit(0);
}




int main()
{
    int p;
    int i;


    srand(getpid());
	
    // Si existe el archivo temporal, lo borra
    sem_unlink("/sem1");


    // Semáforos nombrados
	// Iniciliza datos semáforos
    // sem_open regresa un apuntador al semáforo
    s1 = sem_open("/sem1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);


    // Como el apuntador ya está en la variable s1, todos los procesos que se crean con fork()
    // que heredan los valores de las variables, heredan ese apuntador (s1) 	
    
    for(i=0;i<NPROCS;i++)
    {
        p = fork();
        if(p==0)
            proc(i);
    }


    for(i=0;i<NPROCS;i++)
        wait(NULL);


    sem_close(s1);          // Cerrar el semáforo
    sem_unlink("/sem1");    // Eliminar el archivo temporal del semáforo
}
