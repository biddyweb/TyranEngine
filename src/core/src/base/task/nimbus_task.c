#include "nimbus_task.h"
#include <tyranscript/tyran_memory.h>

nimbus_task* nimbus_task_new(struct tyran_memory* memory, nimbus_task_function work, void* work_self)
{
	nimbus_task* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_task);
	
	self->self = work_self;
	self->work = work;

	return self;
}
