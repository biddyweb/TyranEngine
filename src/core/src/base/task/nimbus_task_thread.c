#include "nimbus_task_thread.h"
#include "nimbus_task_queue.h"
#include <tyran_core/task/task.h>
#include "../thread/nimbus_thread_sleep.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

static void work(void* _self)
{
	nimbus_task_thread* self = _self;

	for (;;) {
		nimbus_task* task = nimbus_task_queue_fetch_next_task(self->task_queue, self->affinity);
		if (task == 0) {
			nimbus_thread_sleep(0.0f);
		} else {
			nimbus_task_call(task, self->task_queue);
			nimbus_task_queue_task_completed(self->task_queue, task);
		}
	}
}

void nimbus_task_thread_init(nimbus_task_thread* self, nimbus_task_queue* task_queue, int affinity)
{
	self->affinity = affinity;
	self->task_queue = task_queue;

	nimbus_thread_init(&self->thread, work, self);
}
