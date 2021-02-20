#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
    pthread_mutex_init(&rw->lock, NULL);
    pthread_rwlock_init(&rw->rw_lock, NULL);
    rw->read = 0;
    rw->write = 0;
}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
    rw->read++;
    pthread_rwlock_rdlock(&rw->rw_lock);
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
    rw->read--;
    pthread_rwlock_unlock(&rw->rw_lock);
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
   while(rw->read) pthread_rwlock_wrlock(&rw->lock);
}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
    pthread_rwlock_unlock(&rw->lock);
}
