#include "nimbus_task.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

void nimbus_task_init(nimbus_task* self, nimbus_task_function work, void* work_self)
{
	TYRAN_ASSERT(work != 0, "Must have a proper function");
	TYRAN_ASSERT(work_self != 0, "Self can not be null");
	self->self = work_self;
	self->work = work;
	self->affinity = -1;
	self->group = 0;
}

void nimbus_task_call(nimbus_task* self, struct nimbus_task_queue* task_queue)
{
	self->work(self->self, task_queue);
}
