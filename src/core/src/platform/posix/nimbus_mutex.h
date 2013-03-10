#ifndef nimbus_mutex_h
#define nimbus_mutex_h

#include <pthread.h>

typedef struct nimbus_mutex {
	pthread_mutex_t mutex;
} nimbus_mutex;

void nimbus_mutex_init(nimbus_mutex* mutex)
{
	pthread_mutex_init(&mutex->mutex, 0);
}

void nimbus_mutex_destroy(nimbus_mutex* mutex)
{
	pthread_mutex_destroy(&mutex->mutex);
}

void nimbus_mutex_lock(nimbus_mutex* mutex)
{
	pthread_mutex_lock(&mutex->mutex);
}

void nimbus_mutex_unlock(nimbus_mutex* mutex)
{
	pthread_mutex_unlock(&mutex->mutex);
}

#endif
