#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define MAX_MSG_SIZE 128
#define MB_SIZE 5
#define MSG_ERROR ((MESSAGE){ .source = -1, .dest = -1, .content = "ERROR" })


typedef struct{
    int source;
    int dest;
    char content[MAX_MSG_SIZE];
} MESSAGE;

typedef struct {
    int in;
    int out;
    int shmid;
    sem_t *s_block;
    sem_t *s_mutex;

    MESSAGE messages[MB_SIZE];
} MAILBOX;


void waitsem(MAILBOX *mb,MESSAGE msg) {
    sem_wait(mb->s_mutex);

    if(mb->in == mb->out && mb->messages[mb->out].content[0] != '\0') {
        printf("blocked\n");
        sem_post(mb->s_mutex);
        sem_wait(mb->s_block);
        sem_wait(mb->s_mutex);
        printf("unblocked\n");
    }

    
    mb->messages[mb->in] = msg;
    printf("message %s saved at %d\n",msg.content, mb->in);
    mb->in = ((mb->in + 1) % MB_SIZE);

    //printf("message sent!\n");
    

    sem_post(mb->s_mutex);
}


MESSAGE signalsem(MAILBOX *mb) {

    int out = mb->out;
    MESSAGE msg;
    int sval;
    // int sem_value;
    // int result;

    sem_wait(mb->s_mutex);

    if(mb->messages[out].content[0] == '\0') {
        sem_post(mb->s_mutex);
        msg = MSG_ERROR;
        return msg;
    }

    else {
    
        msg = (mb->messages[out]);
        mb->out = ((mb->out + 1) % MB_SIZE);
        mb->messages[out].content[0] = '\0';

        if (!sem_getvalue(mb->s_block, &sval)) {
            if (sval <= 0) {
                sem_post(mb->s_block);
            }
        }

        sem_post(mb->s_mutex);
        printf("\t\t\tmessage %s retrieved from %d!\n", msg.content, out);
        return msg;
    }

}


MAILBOX * initsem() {

    int shmid;
    size_t size = 4096;
    MAILBOX * mailbox;

    shmid = shmget((key_t)0x6812, sizeof(MAILBOX), IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {

        shmid = shmget((key_t)0x6812, sizeof(MAILBOX), 0666);

        if (shmid == -1) {
            printf(perror); 
            exit(0);
        }
    }

    mailbox=shmat(shmid,NULL,0);

    sem_unlink("/s_mutex");
    sem_unlink("/s_block");

    mailbox->s_mutex = sem_open("/s_mutex",O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,1); 
    mailbox->s_block = sem_open("/s_block",O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,0);

    mailbox->shmid = shmid;
    mailbox->in = 0;
    mailbox->out = 0;

    for (int i = 0; i < MB_SIZE; i++) {
        mailbox->messages[i].source = -1;
        mailbox->messages[i].dest = -1;
        mailbox->messages[i].content[0] = '\0';
    }

    return mailbox;
}

destroysem(MAILBOX * mb) {

    if (shmctl(mb->shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID failed");
        // Handle error
    }

    sem_close(mb->s_mutex);
    sem_close(mb->s_block);

    sem_unlink("/s_mutex");
    sem_unlink("/s_block");
}

//MAILBOX mb_;

int main(){

    MESSAGE msg;
    MESSAGE msg_return;
    char str[MAX_MSG_SIZE];
    
    MAILBOX * mb = initsem();

    int p = fork();

    if(!p) {
        msg.source = getpid();
        msg.dest = 0;

        for(int i=0; i<MB_SIZE*2;i++) {
            sprintf(str,"TEST%d",i);
            strcpy(msg.content,str);
            waitsem(mb,msg);
            //printf("producer:TEST%d\n", i);
        }
        exit(0);

    }
    
    else{
        sleep(1);
        for(int i=0; i<MB_SIZE*2;i++) {
            msg_return = signalsem(mb);
            if (msg_return.source == -1 ) printf("\t\t\tcontent %s\n",msg_return.content);  
        }
    }

    destroysem(mb);
}


