#ifndef nimbus_mutex_h
#define nimbus_mutex_h

#include <pthread.h>

typedef pthread_mutex_t nimbus_mutex;

#define nimbus_mutex_init(handle) pthread_mutex_init(handle, NULL);
#define nimbus_mutex_destroy(handle) pthread_mutex_destroy(handle);
#define nimbus_mutex_lock(handle) pthread_mutex_lock(handle);
#define nimbus_mutex_unlock(handle) pthread_mutex_unlock(handle);

#endif
