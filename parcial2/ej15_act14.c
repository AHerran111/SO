// Cuatro personas están sentados en una mesa jugando dominó, se van turnando
// para tirar una ficha o pasar si no pueden jugar en el sentido contrario a las 
// manecillas del reloj.
// Represente a los cuatro jugadores con procesos desde P(0) hasta P(3) y como 
// sincronizarían los turnos usando un arreglo de semáforos.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>

#define ROUNDS 5
#define PLAYERS 4


sem_t *sems[PLAYERS];
char * sem_names[PLAYERS];


void turn(int p){

     if(rand() % 2)  printf("Jugador %d Hace jugada\n", p);

     else printf("Jugador %d salta su turno\n", p);
}

void jugador(int p){

    int n = ROUNDS;
    
    printf("soy el jugador %d con el pid %d\n", p,getpid());

    while(n--)  {
        
        sem_wait(sems[p]);  //ENTRA A SC

        printf("Inicio de turno %d del jugador %d\n", ROUNDS-n,p);
        turn(p);
        printf("Termina turno %d del jugador %d\n", ROUNDS-n,p);

        if(p < PLAYERS-1) sem_post(sems[p+1]);   //SALE DE SC 
        else sem_post(sems[0]);   //SALE DE SC 
    }

    exit(0);
}


int main(){
	
    int p;
   
    char sem_name[] = "/sem0"; 
    int len = strlen(sem_name);
    

    for(int i=0; i<PLAYERS; i++) {

      sem_name[len-1] = '0' + i;
      sem_names[i] = strdup(sem_name);

      sem_unlink(sem_name);
    }

    srand(getpid());

    // Semáforos nombrados
	// Iniciliza datos semáforos
    // sem_open regresa un apuntador al semáforo
    sems[0] = sem_open(sem_names[0], O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);

    for(int i=1; i<PLAYERS; i++) {
        sems[i] = sem_open(sem_names[i], O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    }

    for(int i=0; i<PLAYERS; i++) {
        p = fork();
        if (!p)    jugador(i);
    }

    
    for(int i=0; i<PLAYERS; i++) {
       wait(NULL);
    }

    for(int i=0; i<PLAYERS; i++) {
       sem_close(sems[i]);          // Cerrar el semáforo
       sem_unlink(sem_names[i]);    // Eliminar el archivo temporal del semáforo
    }
   

}