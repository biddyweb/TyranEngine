#ifndef nimbus_boot_h
#define nimbus_boot_h

#include <tyranscript/tyran_memory.h>

#include "../task/nimbus_task_thread.h"

struct nimbus_task_queue;
struct nimbus_engine;

typedef struct nimbus_boot {
	struct nimbus_task_queue* task_queue;
	struct nimbus_task_thread task_threads[8];
	struct nimbus_engine* engine;
	int task_thread_count;
	tyran_memory memory;
	u8t* memory_area;
} nimbus_boot;

nimbus_boot* nimbus_boot_new();
void nimbus_boot_destroy(nimbus_boot* boot);

tyran_boolean nimbus_boot_ready_for_next_frame(nimbus_boot* self);
tyran_boolean nimbus_boot_should_render(nimbus_boot* self);
int nimbus_boot_update(nimbus_boot* self);


#endif
