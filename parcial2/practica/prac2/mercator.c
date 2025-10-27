/*           
        Para compilar incluir la librería  m (matemáticas)

        Ejemplo:
            gcc -o mercator mercator.c  -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <mqueue.h>
#include <string.h>


#define NPROCS 4
#define SERIES_MEMBER_COUNT 200000
#define FLAGS 0			// Default
#define MAXMSG 6		// Máximo de mensajes en el buzón
#define MSGSIZE __SIZEOF_DOUBLE__		// Tamaño máximo del mensaje
#define CURMSGS 0		// Mensajes actuales en el buzón
struct mq_attr attr = {FLAGS,MAXMSG,MSGSIZE,CURMSGS};


double get_member(int n, double x)
{
    int i;
    double numerator = 1;

    for(i=0; i<n; i++ )
        numerator = numerator*x;

    if (n % 2 == 0)
        return ( - numerator / n );
    else
        return numerator/n;
}



void proc(int proc_num)
{
    int i;
    double mensaje;
	int priority;
	mqd_t queue_id;	// Buzón de mensajes
    double result = 0; 

    queue_id = mq_open("/mqueue",O_RDWR  | O_CREAT, 0600, &attr);

    // Espera a que master_proc ponga la bandera en 1 para iniciar
	mq_receive(queue_id, (char*)&mensaje, attr.mq_msgsize, &priority);
	printf("\t\t\t%f %d\n", mensaje, proc_num);	// Imprimir el mensaje
		
    // Cada proceso realiza el cálculo de los términos que le tocan
    for( i=proc_num; i<SERIES_MEMBER_COUNT; i+=NPROCS)
        result += get_member (i+1, mensaje);
    
    printf("\t\t\t%f %d\n", result, proc_num);

    // Incrementa la variable proc_count que es la cantidad de procesos que terminaron su cálculo
    mq_send( queue_id, (const char*)&result, attr.mq_msgsize, 0);

    mq_close(queue_id);
    exit(0);
}



void master_proc(double x_val)
{
    int i;
    mqd_t queue_id;	// Buzón de mensajes
    double message = x_val; 
    int priority;
    double sums[NPROCS] = {0.0};
    double result = 0;

    queue_id = mq_open("/mqueue",O_RDWR  | O_CREAT , 0600, &attr);

    // el master_proc pone una bandera para que los procesos inicien con el cálculo
    for(i=0; i<NPROCS; i++) 
        mq_send(queue_id, (const char*)&message, attr.mq_msgsize, 0);


    // Espera a que todos los procesos terminen su cálculo
    for(i=0; i<NPROCS; i++) { 
        mq_receive(queue_id, (char*)&sums[i], attr.mq_msgsize, &priority);
            printf("Recibido %f\n", sums[i]);	// Imprimir el mensaje
    }


    for(i=0; i<NPROCS; i++)
        result += sums[i];

    printf("%10.8f\n", result);

    mq_close(queue_id);

    queue_id = mq_open("/mainqueue",O_RDWR  | O_CREAT , 0600, &attr);

    mq_send(queue_id, (const char*)&result, attr.mq_msgsize, 0);

    mq_close(queue_id);
    exit(0);
}



int main()
{
    long long start_ts_sec = 0;
    long long stop_ts_sec = 0;
    double elapsed_seconds = 0.0;
    struct timeval ts_start, ts_stop;
    int i, p, status;
    mqd_t queue_id;
    int priority;
    double result;
    double x_val;


    mq_unlink("/mainqueue");
    mq_unlink("/mqueue");

    queue_id = mq_open("/mainqueue", O_RDWR | O_CREAT, 0600, &attr);

    
    FILE *fp = fopen("entrada.txt", "r");
    if (fp == NULL) {
        perror("fopen entrada.txt");
        exit(1);
    }

    fscanf(fp, "%lf", &x_val);
    fclose(fp);

    printf("x read: %f\n",x_val);


    gettimeofday(&ts_start, NULL);
    start_ts_sec = ts_start.tv_sec;

    for (i = 0; i < NPROCS; i++) {
        p = fork();
        if (!p) {
            proc(i); 
           // exit(0);
        }
    }


    p = fork();
    if (!p) {
        master_proc(x_val);
       // exit(0);
    }

    mq_receive(queue_id, (char*)&result, attr.mq_msgsize,&priority);

    gettimeofday(&ts_stop, NULL);
    elapsed_seconds = (ts_stop.tv_sec - ts_start.tv_sec) + 
    (ts_stop.tv_usec - ts_start.tv_usec) / 1e6;

    printf("El recuento de ln(1 + x) miembros de la serie de Mercator es %d\n", SERIES_MEMBER_COUNT);
    printf("Tiempo = %.6f segundos\n", elapsed_seconds);
    printf("El resultado es %10.8f\n", result);
    printf("Llamando a la función ln(1 + %f) = %10.8f\n", x_val, log(1 + x_val));

    mq_close(queue_id);

    mq_unlink("/mainqueue");
    mq_unlink("/mqueue");
    
    return 0;
}