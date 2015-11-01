#include "nimbus_thread_windows.h"
#include <tyranscript/tyran_log.h>

static DWORD WINAPI thread_main_routine(LPVOID data)
{
	nimbus_thread* self = (nimbus_thread*) (data);

	self->func(self->other_self);

	return 0;
}

static void start(nimbus_thread* self)
{
	const DWORD stack_size = 0;
	LPVOID thread_parameter = self;
	DWORD creation_flags = 0;
	DWORD thread_id;
	self->handle = CreateThread(0, stack_size, thread_main_routine, thread_parameter, creation_flags, &thread_id);
	TYRAN_ASSERT(self->handle, "Couldn't create a thread");
	SetThreadPriority(self->handle, THREAD_PRIORITY_ABOVE_NORMAL);
	TYRAN_LOG("Thread started!");
}

void nimbus_thread_init(nimbus_thread* self, nimbus_thread_func func, void* other_self)
{
	start(self);
}

tyran_boolean nimbus_thread_is_done(nimbus_thread* self)
{
	return TYRAN_FALSE;
}
