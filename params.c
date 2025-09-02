#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    printf("La cantidad de args:%d\n", argc);
    
    for(int i=0; i<argc; i++) {
        printf("arg %d:%s\n", i, argv[i]);
    }

}