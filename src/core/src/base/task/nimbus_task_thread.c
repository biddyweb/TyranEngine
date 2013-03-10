#include "nimbus_task_thread.h"
#include "nimbus_task_queue.h"
#include "nimbus_task.h"
#include "../thread/nimbus_thread_sleep.h"
#include <tyranscript/tyran_memory.h>

nimbus_task_thread* nimbus_task_thread_new(tyran_memory* memory, nimbus_task_queue* task_queue, int affinity)
{
	nimbus_task_thread* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_task_thread);
	self->affinity = affinity;
	self->task_queue = task_queue;

	return self;
}

void nimbus_task_thread_work(nimbus_task_thread* self)
{
	for (;;) {
		nimbus_task* task = nimbus_task_queue_fetch_next_task(self->task_queue, self->affinity);
		if (task == 0) {
			nimbus_thread_sleep(0.1f);
		} else {
			task->work(task->self);
			nimbus_task_queue_task_completed(self->task_queue, task);
		}
	}
}
