#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

int pid_n[6] = {1};
int pid_h[2] = {1};
int flag = 0;


void child_signal(int sig){

    if (sig == SIGUSR1) {
        for(int i=flag;i<flag+3;i++) {
            printf("Matando al proceso %d\n",pid_n[i]);
            kill(pid_n[i],SIGKILL);
        }
    }
        
}

int main(){
    
    int choice = 0;

    while(choice != 6) {

        int i = 0, f = 0,p = 0, n = 0, status;
        int pid_root = getpid();
        printf("Soy el padre con el pid:%d\n", pid_root);

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


                for(i=0; i<2; i++) {

                    pid_h[i] = fork();

                    if (!pid_h[i])
                        break;
                }

                //printf("%d %d, soy %d\n",pid_h[0],pid_h[1], getpid());

                if (getpid() != pid_root) {
                    printf("Hijo creado con pid %d\n", getpid());
                    signal(SIGUSR1,child_signal);
                    
                    if (pid_h[0]) flag = 3;

                    for(i=flag; i<flag+3; i++) {
                        pid_n[i] = fork();
                        
                        if (!pid_n[i]) {
                            //printf("asignando pid_n[%d] = %d\n", i, getpid());
                            break;
                        }                        
                    }

                    if (getppid() != pid_root) {
                        printf("Nieto creado con pid %d\n", getpid());
                        pause();//SIGKILL
                        
                    }

                    pause();//SIGUSR1
                    pause();//SIGKILL
                }

                else {
                    sleep(2);
                    printf("Matando familia\n");
                    
                    kill(pid_h[0],SIGUSR1);
                    kill(pid_h[1],SIGUSR1);
                    sleep(2);
                    kill(pid_h[0],SIGKILL);
                    printf("Matando al proceso %d\n",pid_h[0]);
                    kill(pid_h[1],SIGKILL);
                    printf("Matando al proceso %d\n",pid_h[1]);
                    
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