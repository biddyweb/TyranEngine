#include "nimbus_task_queue.h"
#include <tyranscript/tyran_memory.h>
#include "nimbus_task.h"
#include "../mutex/nimbus_mutex.h"
#include <tyranscript/tyran_log.h>

nimbus_task_queue* nimbus_task_queue_new(tyran_memory* memory)
{
	nimbus_task_queue* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_task_queue);

	nimbus_mutex_init(&self->mutex);

	for (int i=0; i<8; ++i) {
		self->group_counter[i] = 0;
	}

	self->task_max_count = 128;

	return self;
}

void nimbus_task_queue_destroy(nimbus_task_queue* self)
{
	nimbus_mutex_destroy(&self->mutex);
	TYRAN_MEMORY_FREE(self);
}

void nimbus_task_queue_add_task(nimbus_task_queue* self, nimbus_task* task)
{
	TYRAN_ASSERT(task, "MUST BE zero");
	TYRAN_ASSERT(self->task_count < self->task_max_count, "FULL");
	TYRAN_ASSERT(task->work!=0, "work must not be zero");
	TYRAN_ASSERT(task->self!=0, "self must not be zero in task");
	nimbus_mutex_lock(&self->mutex);

	task->task_queue = self;

	if (task->group != 0) {
		self->group_counter[task->group]++;
	}

	self->tasks[self->task_write_index++] = task;
	self->task_write_index %= self->task_max_count;
	self->task_count++;

	nimbus_mutex_unlock(&self->mutex);
}

tyran_boolean nimbus_task_queue_has_pending_tasks_from_group(nimbus_task_queue* self, int group)
{
	nimbus_mutex_lock(&self->mutex);
	int tasks_left = self->group_counter[group];
	nimbus_mutex_unlock(&self->mutex);

	return (tasks_left != 0);
}

nimbus_task* nimbus_task_queue_fetch_next_task(nimbus_task_queue* self, int requested_affinity)
{
	nimbus_task* task;

	if (self->task_count == 0) {
		task = 0;
	} else {
		nimbus_mutex_lock(&self->mutex);
		task = self->tasks[self->task_read_index];

		if (task->affinity != -1 && requested_affinity != task->affinity) {
			task = 0;
		} else {
			self->tasks[self->task_read_index] = 0;
			self->task_read_index++;
			self->task_read_index %= self->task_max_count;
			self->task_count--;
		}
		nimbus_mutex_unlock(&self->mutex);
	}

	return task;
}

void nimbus_task_queue_task_completed(nimbus_task_queue* self, nimbus_task* task)
{
	if (task->group != 0) {
		TYRAN_ASSERT(self->group_counter[task->group] != 0, "Unmatching group counter:%d ", task->group);
		nimbus_mutex_lock(&self->mutex);
		self->group_counter[task->group]--;
		nimbus_mutex_unlock(&self->mutex);
	}
}


