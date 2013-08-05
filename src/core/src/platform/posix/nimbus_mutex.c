#if defined TORNADO_OS_IOS
#include "nimbus_mutex.h"
#include <pthread.h>

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
#else
int a;
#endif
