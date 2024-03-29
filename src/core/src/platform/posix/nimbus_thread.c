#include "nimbus_thread.h"

#include <tyranscript/tyran_log.h>

void nimbus_thread_done(nimbus_thread* thread)
{
	thread->_is_done = TYRAN_TRUE;
}

void* nimbus_thread_main_routine(void* data)
{
	nimbus_thread* thread = (nimbus_thread*) (data);

	thread->func(thread->other_self);
	nimbus_thread_done(thread);

	return 0;
}

void nimbus_thread_init(nimbus_thread* self, nimbus_thread_func func, void* other_self)
{
	self->other_self = other_self;
	self->func = func;
	int return_code = pthread_attr_init(&self->attr);
	if (return_code) {
		return;
	}
	TYRAN_ASSERT(return_code == 0, "pthread_attr_init() failed");
	return_code = pthread_attr_setdetachstate(&self->attr, PTHREAD_CREATE_DETACHED);
	TYRAN_ASSERT(return_code == 0, "setdetachstate() failed");

	int threadError = pthread_create(&self->thread_id, &self->attr, &nimbus_thread_main_routine, self);
	return_code = pthread_attr_destroy(&self->attr);
	TYRAN_ASSERT(return_code == 0, "attr_destroy() failed");

	if (threadError != 0) {
		TYRAN_ERROR("ERRROR!!!!");
	}
}

tyran_boolean nimbus_thread_is_done(nimbus_thread* self)
{
	return self->_is_done;
}
