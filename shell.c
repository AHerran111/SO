#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "format.h"

void sigchild_handler (int sig) {
    int status, pid;


    //waitpid runs constantly returning 0, when it catches a zombie it returns 1
    //on loop to guarantee it cathces all zombies, SIGCHILD signals may be not 1:1
    //by catching the signal the kernel cleans up, the child is already exited
    while (pid = waitpid(-1,&status,WNOHANG) > 0) {
                                              
        printf("zombie aniquilado con pid:%d\n", pid);
    }
}

int main() {
    int i = 0, f, status;
    char input[100];
    char *command[20];    
    char *token;
  
    system("clear"); 
    
    while(strcmp("exit",input)) {
        i = 0;

        printf(GREEN("HERRAN_ROMERO@shell")">>$ ");
       
        fgets(input,sizeof(input),stdin);
        input[strcspn(input, "\n")] = '\0';//removes trailing "enter"

        token = strtok(input, " ");

        while(token) {

            command[i] = token;
            token = strtok(NULL, " ");
            //It keeps track of where it left off using internal static memory, 
            // so you keep calling it with NULL until no tokens remain. 

            i += 1;
        }
        
        command[i] = NULL;//execvp() recieves the last NULL in the array
        
        f = fork();

        if (f < 0)  perror("fork fallido");
        
        else {
            if(strcspn(command[i-1],"&") == 0) {

                if (f) printf("Proceso en background(PID: %d)\n", f);
            
                else {

                    setsid(); //creating deamon for background proccess
                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    close(STDERR_FILENO);

                    execvp(command[0],command);
                    perror("Falla en ejecucion excecvp()");
                    exit(0);
                }
                sleep(1);
            }

            else {
                if (!f) {
                    execvp(command[0],command);
                    perror("Falla en ejecucion excecvp()");
                    exit(0);
                }
                waitpid(f,&status,0);
            }
        }
        
        memset(command, 0, sizeof(command));
        memset(input, 0, sizeof(input));
    }

}