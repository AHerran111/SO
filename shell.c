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
    char *p_command[20];  
    char *token;
    int p_flag = 0;
    int des_p[2];
  
    system("clear"); 
    signal(SIGCHLD, sigchild_handler);

    while(strcmp("exit",input)) {
        i = 0, p_flag = 0;

        printf(GREEN("HERRAN_ROMERO@shell")">>$ ");
       
        fgets(input,sizeof(input),stdin);
        input[strcspn(input, "\n")] = '\0';//removes trailing "enter"

        token = strtok(input, " ");

        while(token) {

            if (strcspn(token,"|") == 0) {
              printf("pipe detected\n");
              p_flag = 1;
              break;
            }

            command[i] = token;
            token = strtok(NULL, " ");
            //It keeps track of where it left off using internal static memory, 
            // so you keep calling it with NULL until no tokens remain. 
        
            i += 1;
        }
        command[i] = NULL;//execvp() recieves the last NULL in the array

        if (p_flag) {
            i = 0;

            while (token) {

                token = strtok(NULL, " ");
                p_command[i] = token;
                
                i += 1;
            }
            p_command[i] = NULL;//execvp() recieves the last NULL in the array
        }
        
        if (!p_flag) {

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
        }

        else {
            ///taken from:
            //https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection

            if (pipe(des_p) == -1) {
                perror("Pipe failed");
                exit(1);
            }

            if (fork() == 0)            //first fork
            {
                close(STDOUT_FILENO);  //closing stdout
                dup(des_p[1]);         //replacing stdout with pipe write 
                close(des_p[0]);       //closing pipe read
                close(des_p[1]);

                execvp(command[0],command);
                perror("Falla en ejecucion excecvp()");
                exit(0);
            }

            if (fork() == 0)            //creating 2nd child
            {
                close(STDIN_FILENO);   //closing stdin
                dup(des_p[0]);         //replacing stdin with pipe read
                close(des_p[1]);       //closing pipe write
                close(des_p[0]);

                execvp(p_command[0],p_command);
                perror("Falla en ejecucion excecvp()");
                exit(0);
            }

            close(des_p[0]);
            close(des_p[1]);
            wait(0);
            wait(0);
        }
        
        memset(command, 0, sizeof(command));
        memset(p_command, 0, sizeof(command));
        memset(input, 0, sizeof(input));
    }

}