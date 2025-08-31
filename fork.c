//     1.-Escribir un programa que cree un hijo. El hijo debe escribir: "Soy el hijo" diez veces.
//     El padre debe escribir: "Soy el padre" diez veces. Ambos procesos deberán mostrar 
//     concurrentemente sus líneas en pantalla y entre cada linea que escriban deben hacer
//     una pausa de un segundo. El padre debe esperar a que termine el hijo y mostrar el
//     mensaje: "Mi proceso hijo ya ha terminado".


//     2.-Desarrolla un programa que reciba como argumento desde la línea de comandos un valor 
//     de entrada n cuyo significado sera nivel. Ese nivel tendrá el significado de cuántos
//     niveles tendrá un árbol de procesos y al finalizar deberá haber 2n procesos donde 
//     cada proceso deberá ir mostrando en pantalla  un número que será el nivel en el que
//     se encuentre actualmente, de manera que cada uno de los procesos estará mostrando en
//     pantalla todos los niveles por los que va pasando. Por ejemplo:

//     Si n=1 terminarán dos procesos que deberán mostrar en pantalla 0,1,1
//     Si n=2 terminarán cuatro procesos que deberán mostrar en pantalla 0,1,1,2,2,2,2
//     Si n=4 terminarán 16 procesos que deberán mostrar en pantalla 
//     0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void child(int n){

    printf("soy el hijo i=%d\n",n);

    exit(0);
}

int main(){

    int choice;

    do {
        choice = 0;
        int i = 0, f = 0,p = 0;

        printf(
            "Bienvenido a Actividad 3, ingrese la tarea a correr\n"
            "1.ejer1\n"
            "2.ejer2\n"
            "3.ejer3\n"
            "4.ejer4\n"
            "5.ejer5\n"
            "6.exit\n"
        );
        scanf("%d",&choice);


        switch(choice)
        {
            case 1: {
                for(i=0;i<11;i++){
                    f = fork();
                    
                    if (f==0) child(i);

                    else printf("soy el padre i=%d\n",i);

                    sleep(1);
                }

                break;
            }

            case 2: {

                int n;

                printf("Ingresa la cantidad de niveles: ");
                scanf("%d",&n);

                for(i=0; i<=n; i++) {
                   
                    printf("%d\n",i);
                    f = fork();
                }

                exit(0);
                break;
            }

            case 3: {

            }

            case 6:
                printf("byeee\n");
                break;

            default:
            printf("Opcion incorrecta\n"); break;
        }

        
    }while(choice != 6);

}