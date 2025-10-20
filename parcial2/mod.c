#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    int i = 0;
    int n = 5;
    while(1) {
        printf("a:%d\n",i);
        i = (i+1) % 5;
        printf("b:%d\n",i);
        getchar();
    }
}