#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>   
#include "format.h"

int f;

void sigchild_handler (int sig) {
    int status, pid;


    //waitpid runs constantly returning 0, when it catches a zombie it returns 1
    //on loop to guarantee it cathces all zombies, SIGCHILD signals may be not 1:1
    //by catching the signal the kernel cleans up, the child is already exited
    while (waitpid(-1,&status,WNOHANG) > 0) {
                                              
        printf(BLUE("zombie aniquilado\n"));
    }
}

void child_sigint_handler (int sig) {
    if(f) {
        kill(f,SIGKILL);
    }
}



int main() {
    int i = 0, status;
    char input[100]; //scanf input
    char *command[20];   //parsed command
    char *token; 

    char *p_command[20];  //pipe vars;
    int p_flag = 0; 
    int des_p[2]; 

    int in_flag = 0, out_flag = 0; //variables for redirect
    char *in_file = malloc(64); 
    char *out_file = malloc(64); 
    int saved_stdin;
    int saved_stdout; 
   
    system("clear"); 
    signal(SIGCHLD, sigchild_handler);

    while(1) {

        i = 0, p_flag = 0, in_flag = 0, out_flag = 0;

        signal(SIGINT, SIG_IGN);

        printf(GREEN("HERRAN_SILVA@shell")">>$ ");
       
        fgets(input,sizeof(input),stdin);
        input[strcspn(input, "\n")] = '\0';//removes trailing "enter"

        if (strcspn(input,"\0") == 0) continue;
        if (strcspn(input,"exit") == 0) return 0;

        //stdin and stdout are restorded in case of redirect
        saved_stdin = dup(STDIN_FILENO);
        saved_stdout = dup(STDOUT_FILENO);

        //parsing command
        token = strtok(input, " ");

        while(token) {

            if (strcspn(token,"|") == 0) {
              printf(BLUE("pipe detected\n"));
              p_flag = 1;
              break;
            }

            if (strcspn(token,"<") == 0) {

                printf(BLUE("sdtin redirect detected\n"));
                in_flag = 1;
                command[i] = NULL;
                break;
            }

            if (strcspn(token,">") == 0) {

                printf(BLUE("sdtout redirect detected\n"));
                out_flag = 1;
                command[i] = NULL;
                break;
            }

            command[i] = token;
            token = strtok(NULL, " ");
            //It keeps track of where it left off using internal static memory, 
            // so you keep calling it with NULL until no tokens remain. 
        
            i += 1;
        }
        command[i] = NULL;//execvp() recieves the last NULL in the array


        //parsing 2nd command after pipe
        if (p_flag) {
            i = 0;

            while (token) {

                token = strtok(NULL, " ");
                p_command[i] = token;
                
                i += 1;
            }
            p_command[i] = NULL;
        }

        //parsing file names after redirect
        if (in_flag || out_flag) {

            token = strtok(NULL, " ");
            
            if (in_flag) in_file = token;
            else out_file= token;

        }
        
        if (!p_flag) {

            f = fork();

            if (f < 0)  perror(RED("fork fallido"));
            
            else {

                signal(SIGINT, child_sigint_handler); //function kills child

                //background process
                if(strcspn(command[i-1],"&") == 0) {

                    if (f) printf(BLUE("Proceso en background(PID: %d)\n"), f);
                
                    else {

                        //creating deamon for background proccess
                        setsid(); 
                        close(STDIN_FILENO);
                        close(STDOUT_FILENO);
                        close(STDERR_FILENO);

                        execvp(command[0],command);
                        perror(RED("Falla en ejecucion background excecvp()"));
                        exit(0);
                    }
                    sleep(1);
                }
                
                else {

                    //redirects
                    //taken from
                    //https://stackoverflow.com/questions/11515399/implementing-shell-in-c-and-need-help-handling-input-output-redirection
                    if (in_flag)
                    {
                        int fd0 = open(in_file, O_RDONLY); //opens file
                        dup2(fd0, STDIN_FILENO); //copies descriptor to stdin
                        close(fd0); ///closes file
                    }

                    else if (out_flag)
                    {
                        //0644 creates or opens file
                        int fd1 = creat(out_file , 0644); 
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);
                    }
                    
                    //normal exec
                    if (!f) {
                        execvp(command[0],command);
                        perror(RED("Falla en ejecucion excecvp()"));
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

            if (fork() == 0) //first fork
            {
                //first pipe writes to second pipe
                //closing stdout
                dup2(des_p[1], STDOUT_FILENO); //replacing stdout with pipe write 
                close(des_p[0]); //closing pipe read
                close(des_p[1]);

                execvp(command[0],command);
                perror(RED("Falla en ejecucion pipe 1 excecvp()"));
                exit(0);
            }

            if (fork() == 0) //creating 2nd child
            {
                //second pipe reads from first pipe
                //closing stdin
                dup2(des_p[0], STDIN_FILENO); //replacing stdin with pipe read
                close(des_p[1]); //closing pipe write
                close(des_p[0]);

                execvp(p_command[0],p_command);
                perror(RED("Falla en ejecucion pipe 2 excecvp()"));
                exit(0);
            }

            close(des_p[0]);
            close(des_p[1]);
            wait(0);
            wait(0);
        }


        //cleaning up
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdin);
        close(saved_stdout);
        memset(out_file, 0, sizeof(out_file));
        memset(in_file, 0, sizeof(in_file));
        memset(command, 0, sizeof(command));
        memset(p_command, 0, sizeof(command));
        memset(input, 0, sizeof(input));
    }

    free(in_file);
    free(out_file);
}