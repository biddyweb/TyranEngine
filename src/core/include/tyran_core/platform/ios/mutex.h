#ifndef nimbus_mutex_h
#define nimbus_mutex_h

#include <tornado/posix_threads.h>

#define nimbus_mutex pthread_mutex_t;

#define nimbus_mutex_init(handle) pthread_mutex_init(handle, NULL);
#define nimbus_mutex_destroy(handle) pthread_mutex_destroy(handle);
#define nimbus_mutex_lock(handle) pthread_mutex_lock(handle);
#define nimbus_mutex_unlock(handle) pthread_mutex_unlock(handle);

#endif
