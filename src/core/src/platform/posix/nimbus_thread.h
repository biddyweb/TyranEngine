#ifndef nimbus_thread_h
#define nimbus_thread_h

#include <pthread.h>

#include <tyranscript/tyran_types.h>

struct tyran_memory;

typedef void (*nimbus_thread_func)(void* self);

typedef struct nimbus_thread {
	pthread_attr_t attr;
	pthread_t thread_id;
	tyran_boolean _is_done;
	nimbus_thread_func func;
	void* self;
} nimbus_thread;

nimbus_thread* nimbus_thread_new(struct tyran_memory* memory, nimbus_thread_func func, void* self);
tyran_boolean nimbus_thread_is_done(nimbus_thread* thread);

#endif
