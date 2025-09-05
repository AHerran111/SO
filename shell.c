#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int i = 0,f;
    int status;
    char input[100];
    char *command[20];    
    char* token;
  

    while(strcmp("exit",input)) {
        i = 0;

        printf("HERRAN_ROMERO@shell>>");
       
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

                    setsid();
                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    close(STDERR_FILENO);

                    execvp(command[0],command);
                    perror("Falla en ejecucion excecvp()");
                    exit(0);
                }
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
        
        sleep(1);
        memset(command, 0, sizeof(command));
        memset(input, 0, sizeof(input));
    }

}