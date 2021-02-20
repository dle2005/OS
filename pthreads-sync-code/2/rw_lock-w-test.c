#include "rw_lock.h"
pthread_cond_t rw_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t rw_write = PTHREAD_COND_INITIALIZER;

void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
    pthread_mutex_init(&rw->lock, NULL);
    rw->read = 0;
    rw->write = 0;
    rw->write_state = 0;
}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
    pthread_mutex_lock(&rw->lock);

    while(1) {
        if(rw->write || rw->write_state)
            pthread_cond_wait(&rw_read, &rw->lock);
        else {
            rw->read++;
            pthread_mutex_unlock(&rw->lock);
            break;
        }
    }
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
    pthread_mutex_lock(&rw->lock);

    rw->read--;
    if(!rw->read) pthread_cond_signal(&rw_write);

    pthread_mutex_unlock(&rw->lock);
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
    pthread_mutex_lock(&rw->lock);

    while(1) {
        if(rw->read || rw->write_state) {
            rw->write++;
            pthread_cond_wait(&rw_write, &rw->lock);
        }
        else {
            rw->write--;
            rw->write_state = 1;
            pthread_mutex_unlock(&rw->lock);
            break;
        }
    }
}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
    pthread_mutex_lock(&rw->lock);

    rw->write_state = 0;
    pthread_cond_signal(&rw_write);
    pthread_cond_broadcast(&rw_read);

    pthread_mutex_unlock(&rw->lock);
}
