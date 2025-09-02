#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

int pid_n;
int pid_h;


void child_signal(int sig){

    if (sig == SIGUSR1) {
        kill(pid_n,SIGKILL);
    }
        
}

int main(){
    
    int choice = 0;

    while(choice != 6) {

        int i = 0, f = 0,p = 0, n = 0, status;
        printf("Soy el padre con el pid:%d\n", getpid());

        printf(
            "Bienvenido a Actividad 4, ingrese la tarea a correr\n"
            "1.ejer1\n"
            "2.ejer2\n"
            "3.ejer3\n"
            "4.ejer4\n"
            "6.exit\n"
        );
        scanf("%d",&choice);


        switch(choice)
        {
            case 1: {

                printf("Soy el padre con el pid:%d\n", getpid());

                p = getpid();
                f = fork();
                
                if (f == 0) {
                    printf("Matando al padre...%d\n", p);
                    kill(p, 9);
                    exit(0);
                }
                
                waitpid(f,&status,0);
                //printf("%d %d\n",f, getpid());
                break;
            }


            case 2: {

                sleep(20);
                f = fork();

                if (!f) {
                    sleep(5);
                    exit(0);
                }
                break;
            }

            case 3: {
                
                sleep(5);
                f = fork();

                if (!f) {
                    sleep(20);
                    exit(0);
                }
                break;
            }

            case 4: {

                pid_h = fork();

                if (!pid_h) {
                    printf("Hijo creado con pid %d\n", getpid());
                    signal(SIGUSR1,child_signal);
                    
                    pid_n = fork();

                    if (!pid_n) {
                        printf("Nieto creado con pid %d\n", getpid());
                        pause();//SIGKILL
                        
                    }
                    pause();//SIGUSR1
                    pause();//SIGKILL
                }

                else {
                    sleep(2);
                    printf("Matando familia\n");
                  
                    kill(pid_h,SIGUSR1);
                    sleep(1);
                    kill(pid_h,SIGKILL);
                    
                }
                break;
            }
                
            case 6:
                return 0;

            default:
            printf("Opcion incorrecta\n"); break;
        }
    }
}