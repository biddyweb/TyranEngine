#include "nimbus_thread.h"

#include <tyranscript/tyran_log.h>

void nimbus_thread_done(nimbus_thread* thread)
{
	thread->_is_done = TYRAN_TRUE;
}

void* nimbus_thread_main_routine(void* data)
{
	nimbus_thread* thread = (nimbus_thread*)(data);

	thread->func(thread->self);
	nimbus_thread_done(thread);

	return 0;
}

nimbus_thread* nimbus_thread_new(tyran_memory* memory, nimbus_thread_func func, void* self)
{
	nimbus_thread* thread = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_thread);

	int return_code = pthread_attr_init(&thread->attr);
	TYRAN_ASSERT(return_code == 0, "pthread_attr_init() failed");
	return_code = pthread_attr_setdetachstate(&thread->attr, PTHREAD_CREATE_DETACHED);
	TYRAN_ASSERT(return_code == 0, "setdetachstate() failed");

	int threadError = pthread_create(&thread->thread_id, &thread->attr, &nimbus_thread_main_routine, thread);
	return_code = pthread_attr_destroy(&thread->attr);
	TYRAN_ASSERT(return_code == 0, "attr_destroy() failed");

	if (threadError != 0) {
		TYRAN_ERROR("ERRROR!!!!");
	}

	return thread;
}

tyran_boolean nimbus_thread_is_done(nimbus_thread* thread)
{
	return thread->_is_done;
}
