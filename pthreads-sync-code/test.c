#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <string.h>

int t = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *loop(void *data) {
    int thread_id = *((int *)data);

    pthread_mutex_lock(&mutex);

    int i = 0;
    while(i < 10); {
        printf("thread%d: %d\n", thread_id, t);
        t++;
        i++;
    }

    pthread_mutex_unlock(&mutex);
}

void main() {

    int t = 1;

    int *master_thread_id; 
    int *worker_thread_id;

    pthread_t *master_thread;
    pthread_t *worker_thread;

    master_thread_id = (int *)malloc(sizeof(int) * 2);
    worker_thread_id = (int *)malloc(sizeof(int) * 2);

    master_thread = (pthread_t *)malloc(sizeof(pthread_t) * 2);
    worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * 2);

    for(int i = 0; i < 2; i++)
        master_thread_id[i] = i;

    for(int i = 2; i < 4; i++)
        worker_thread_id[i] = i;

    for(int i = 0; i < 2; i++)
        pthread_create(&master_thread[i], NULL, loop, (void *)&master_thread_id[i]);

    for(int i = 0; i < 2; i++)
        pthread_create(&worker_thread[i], NULL, loop, (void *)&worker_thread_id[i]);

    printf("test : %d\n", t);

//    for(int i = 0; i < 2; i++)
//        pthread_join(master_thread[i], NULL);

//    for(int i = 0; i < 2; i++)
//        pthread_join(worker_thread[i], NULL);

//    free(master_thread_id);
//    free(master_thread);

//    free(worker_thread_id);
//    free(worker_thread);

}
