#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NBUFF 5
#define NITEMS 10

struct {
    int buff[NBUFF];
    sem_t *mutex;
    sem_t *nempty;
    sem_t *nstored;
    int in;
    int out;
} shared;

void *produce(void *arg);
void *consume(void *arg);

int main(int argc, char **argv) {
    pthread_t tid_produce, tid_consume;

    shared.mutex = sem_open("/mutex", O_CREAT, 0644, 1);
    shared.nempty = sem_open("/nempty", O_CREAT, 0644, NBUFF);
    shared.nstored = sem_open("/nstored", O_CREAT, 0644, 0);

    shared.in = 0;
    shared.out = 0;

    pthread_create(&tid_produce, NULL, produce, NULL);
    pthread_create(&tid_consume, NULL, consume, NULL);

    pthread_join(tid_produce, NULL);
    pthread_join(tid_consume, NULL);

    sem_unlink("/mutex");
    sem_unlink("/nempty");
    sem_unlink("/nstored");

    return 0;
}

void *produce(void *arg) {
    int i;
    for (i = 0; i < NITEMS; i++) {
        sem_wait(shared.nempty);
        sem_wait(shared.mutex);

        shared.buff[shared.in] = i;
        printf("Производитель: положил %d в буфер[%d]\n", i, shared.in);
        shared.in = (shared.in + 1) % NBUFF;

        sem_post(shared.mutex);
        sem_post(shared.nstored);

        sleep(1);
    }
    return NULL;
}

void *consume(void *arg) {
    int i, item;
    for (i = 0; i < NITEMS; i++) {
        sem_wait(shared.nstored);
        sem_wait(shared.mutex);

        item = shared.buff[shared.out];
        printf("Потребитель: извлек %d из буфера[%d]\n", item, shared.out);
        shared.out = (shared.out + 1) % NBUFF;

        sem_post(shared.mutex);
        sem_post(shared.nempty);

        sleep(1);
    }
    return NULL;
}

