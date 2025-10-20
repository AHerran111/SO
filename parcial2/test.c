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
#include <errno.h>

#define MAX_MSG_SIZE 128
#define MB_SIZE 5
#define MSG_ERROR ((MESSAGE){ .source = -1, .dest = -1, .content = "ERROR" })

sem_t *s_block;
sem_t *s_mutex;

typedef struct{
    int source;
    int dest;
    char content[MAX_MSG_SIZE];
} MESSAGE;

typedef struct {
    int in;
    int out;
    int box_size;
    MESSAGE messages[MB_SIZE];
} MAILBOX;

void waitsem(MAILBOX *mb,MESSAGE *msg) {
    sem_wait(s_mutex);

    if(mb->in == mb->out && mb->messages[mb->out].content[0] != '\0') {
        sem_post(s_mutex);
        sem_wait(s_block);
        sem_wait(s_mutex);
    } else {
        mb->messages[mb->in] = *msg;
        // printf("stored at %d\n", mb->in);
        mb->in = ((mb->in + 1) % mb->box_size);
        // printf("message sent!\n");
    }

    sem_post(s_mutex);
}

MESSAGE signalsem(MAILBOX *mb) {
    int out = mb->out;
    MESSAGE msg;

    sem_wait(s_mutex);

    if(mb->messages[out].content[0] == '\0') {
        sem_post(s_mutex);
        msg = MSG_ERROR;
        return msg;
    } else {
        msg = (mb->messages[out]);
        mb->out = ((mb->out + 1) % mb->box_size);
        mb->messages[out].content[0] = '\0';

        sem_post(s_block);
        sem_post(s_mutex);
        // printf("message retrieved!\n");
        return msg;
    }
}

void init_mailbox(MAILBOX *mailbox, int size) {
    mailbox->in = 0;
    mailbox->out = 0;
    mailbox->box_size = size;

    for (int i = 0; i < size; i++) {
        mailbox->messages[i].source = -1;
        mailbox->messages[i].dest = -1;
        mailbox->messages[i].content[0] = '\0';
    }
}

int main() {
    /* Named semaphores: unlink first (best-effort), then open */
    sem_unlink("/s_mutex");
    sem_unlink("/s_block");
    s_mutex = sem_open("/s_mutex", O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (s_mutex == SEM_FAILED) {
        perror("sem_open s_mutex");
        exit(1);
    }
    s_block = sem_open("/s_block", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (s_block == SEM_FAILED) {
        perror("sem_open s_block");
        sem_close(s_mutex);
        exit(1);
    }

    int shmid;
    int created = 0;
    key_t key = (key_t)0x6812;
    size_t shm_size = sizeof(MAILBOX);

    /* Try to create the segment exclusively so we know whether we own initialization */
    shmid = shmget(key, shm_size, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid >= 0) {
        created = 1; /* we created it and should initialize */
    } else {
        if (errno == EEXIST) {
            /* already exists, obtain existing id (no EXCL) */
            shmid = shmget(key, shm_size, 0666);
            if (shmid == -1) {
                perror("shmget existing");
                goto cleanup;
            }
        } else {
            perror("shmget create");
            goto cleanup;
        }
    }

    /* Now attach: check return value */
    MAILBOX *mb = shmat(shmid, NULL, 0);
    if (mb == (void *) -1) {
        perror("shmat");
        goto cleanup;
    }

    /* Initialize mailbox only if we created it */
    if (created) {
        init_mailbox(mb, MB_SIZE);
    } else {
        /* Optionally: sanity-check the box_size field and adjust if needed */
        if (mb->box_size != MB_SIZE) {
            fprintf(stderr, "Warning: existing mailbox box_size=%d differs from MB_SIZE=%d\n",
                    mb->box_size, MB_SIZE);
            /* You can decide to reinit only if safe / desired */
        }
    }

    MESSAGE message;
    MESSAGE *msg = &message;
    MESSAGE msg_return;
    char str[MAX_MSG_SIZE];

    msg->source = getpid();
    msg->dest = 0;

    int p = fork();
    if (p < 0) {
        perror("fork");
    } else if (p == 0) {
        /* child: produce MB_SIZE+2 messages (some will block) */
        for(int i=0; i<MB_SIZE+2; i++) {
            snprintf(str, sizeof(str), "TEST%d", i);
            strcpy(msg->content,str);
            waitsem(mb,msg);
        }
        _exit(0);
    } else {
        sleep(1);
        for(int i=0; i<MB_SIZE; i++) {
            msg_return = signalsem(mb);
            if (msg_return.content[0] != '\0' )
                printf("content %s\n", msg_return.content);
        }
        wait(NULL);
    }

    /* detach */
    if (shmdt(mb) == -1) {
        perror("shmdt");
    }

    /* If we created the segment, remove it now */
    if (created) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID failed");
        }
    }

cleanup:
    sem_close(s_mutex);
    sem_close(s_block);
    sem_unlink("/s_mutex");
    sem_unlink("/s_block");

    return 0;
}
