#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
    pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->read_lock, NULL);
    pthread_cond_init(&rw->write_lock, NULL);
    rw->num_read = rw->num_write = 0;
}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
    pthread_mutex_lock(&rw->lock);
    while(rw->num_write) pthread_cond_wait(&rw->read_lock, &rw->lock);
    rw->num_read++;
    pthread_mutex_unlock(&rw->lock);
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
    pthread_mutex_lock(&rw->lock);
    rw->num_read--; 
    if(!rw->num_read) pthread_cond_signal(&rw->write_lock);
    pthread_mutex_unlock(&rw->lock);
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
    pthread_mutex_lock(&rw->lock);
    while(rw->num_read) pthread_cond_wait(&rw->write_lock, &rw->lock);
    rw->num_write++;
    pthread_mutex_unlock(&rw->lock);
}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
    pthread_mutex_lock(&rw->lock);
    rw->num_write--;
    if(!rw->num_write) pthread_cond_signal(&rw->write_lock);
    pthread_mutex_unlock(&rw->lock);
}
