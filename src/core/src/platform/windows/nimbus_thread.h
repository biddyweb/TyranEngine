#ifndef nimbus_thread_h
#define nimbus_thread_h

#include <tyran_core/platform/windows/tyran_windows.h>
#include <tyranscript/tyran_types.h>

typedef void (*nimbus_thread_func)(void* self);

typedef struct nimbus_thread {
	HANDLE handle;
	tyran_boolean _is_done;
	nimbus_thread_func func;
	void* other_self;
} nimbus_thread;

void nimbus_thread_init(nimbus_thread* self, nimbus_thread_func func, void* other_self);
tyran_boolean nimbus_thread_is_done(nimbus_thread* self);

#endif
