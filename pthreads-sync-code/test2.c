#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *loop(void *data) {
    int thread_id = *((int *)data);
    int i = 0;
    pthread_mutex_lock(&mutex);

    while(i < 3) {
        printf("%d\n", i);
        i++;
        sleep(1);
    }

    pthread_mutex_unlock(&mutex);

}

void main() {
    pthread_t my_thread[2];
    int pid = 1;

    pthread_mutex_init(&mutex, NULL);
    
    int my_thread_id = pthread_create(&my_thread[0], NULL, loop, (void *)&pid);

    sleep(1);

    if(my_thread_id < 0) {
        perror("perror\n");
        exit(0);
    }

    printf("my_thread_id : %d\n", my_thread_id);

    pthread_join(my_thread[0], NULL);
}
