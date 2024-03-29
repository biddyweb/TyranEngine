#include "nimbus_boot.h"

#include "../task/nimbus_task_queue.h"
#include "../task/nimbus_task_thread.h"

#include "../../../../engine/base/nimbus_engine.h"

#include "tyranscript/tyran_clib.h"
#include "tyranscript/tyran_log.h"

void nimbus_boot_logger(nimbus_boot* self)
{
}

void nimbus_boot_task_queue(nimbus_boot* self)
{
	self->task_queue = nimbus_task_queue_new(&self->memory);
}

void nimbus_boot_task_threads(nimbus_boot* self)
{
	self->task_thread_count = 3;
	TYRAN_LOG("Tyran Engine v0.0.2 boot. Starting %d thread(s).", self->task_thread_count);
	for (int i = 1; i < self->task_thread_count; ++i) {
		nimbus_task_thread_init(&self->task_threads[i], self->task_queue, i);
	}
	TYRAN_LOG("Tasks booted.");
}

void nimbus_boot_engine(nimbus_boot* self)
{
	self->engine = nimbus_engine_new(&self->memory, self->task_queue);
}

nimbus_boot* nimbus_boot_new()
{
	tyran_memory memory;

	const int max_size = 64000;
	u8t* memory_area = (u8t*) tyran_malloc(max_size);
	tyran_memory_construct(&memory, memory_area, max_size);
	nimbus_boot* self = TYRAN_MEMORY_CALLOC_TYPE(&memory, nimbus_boot);
	self->memory = memory;
	self->memory_area = memory_area;

	nimbus_boot_logger(self);
	nimbus_boot_task_queue(self);
	nimbus_boot_task_threads(self);
	nimbus_boot_engine(self);

	TYRAN_LOG("Boot done!");

	return self;
}

void nimbus_boot_destroy(nimbus_boot* self)
{
	nimbus_engine_free(self->engine);
	tyran_free(self->memory_area);
}

void nimbus_boot_manually_update_affinity_zero_tasks(nimbus_boot* self)
{
	const int affinity = 0;
	for (;;) {
		nimbus_task* task = nimbus_task_queue_fetch_next_task_from_affinity(self->task_queue, affinity);
		if (task == 0) {
			break;
		} else {
			nimbus_task_call(task, self->task_queue);
			nimbus_task_queue_task_completed(self->task_queue, task);
		}
	}
}

tyran_boolean nimbus_boot_ready_for_next_frame(nimbus_boot* self)
{
	tyran_boolean pending_tasks = nimbus_task_queue_has_pending_tasks_from_group(self->task_queue, 1);
	return pending_tasks == 0;
}

tyran_boolean nimbus_boot_should_render(nimbus_boot* self)
{
	tyran_boolean did_draw_something = nimbus_engine_should_render(self->engine);
	return did_draw_something;
}

void nimbus_boot_send_event(nimbus_boot* self, u8t event_id, void* data, int octet_count)
{
	nimbus_engine_send_event(self->engine, event_id, data, octet_count);
}

int nimbus_boot_update(nimbus_boot* self)
{
	int err = 0;
	nimbus_boot_manually_update_affinity_zero_tasks(self);
	if (nimbus_boot_ready_for_next_frame(self)) {
		err = nimbus_engine_update(self->engine, self->task_queue);
	}

	return err;
}

tyran_boolean nimbus_boot_all_threads_are_terminated(nimbus_boot* self)
{
	for (int i = 0; i < self->task_thread_count; ++i) {
		if (!nimbus_thread_is_done(&self->task_threads[i].thread)) {
			return TYRAN_FALSE;
		}
	}

	return TYRAN_TRUE;
}
