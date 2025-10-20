#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/stat.h>

sem_t *s1;

void print(char *s)
{
    usleep(rand()%1000);
    printf("%s\n",s);
    usleep(rand()%1000);
}

void P()
{
    sem_wait(s1);
    print("A");
    print("B");
    sem_post(s1);
    print("C");
    exit(0);
}

void Q()
{
    sem_wait(s1);
    sem_post(s1);
    print("D");
    print("E");
    exit(0);
}

int main()
{
    int p;


    srand(getpid());
	
    // Si existe el archivo temporal, lo borra
    sem_unlink("/sem1");


    // Semáforos nombrados
	// Iniciliza datos semáforos
    // sem_open regresa un apuntador al semáforo
    s1 = sem_open("/sem1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);


    // Como el apuntador ya está en la variable s1, todos los procesos que se crean con fork()
    // que heredan los valores de las variables, heredan ese apuntador (s1) 	
    for(int i=0;i<100;i++){ 
        p=fork();
        if(p==0)
            P();

        p=fork();
        if(p==0)
            Q();

        if(!p) exit(0);

        wait(NULL);
        wait(NULL);
    }

    
    sem_close(s1);          // Cerrar el semáforo
    sem_unlink("/sem1");    // Eliminar el archivo temporal del semáforo
}