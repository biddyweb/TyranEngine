#ifndef nimbus_boot_h
#define nimbus_boot_h

#include <tyranscript/tyran_memory.h>

struct nimbus_task_queue;
struct nimbus_task_thread;
struct nimbus_engine;

typedef struct nimbus_boot {
	struct nimbus_task_queue* task_queue;
	struct nimbus_task_thread* task_threads[8];
	struct nimbus_engine* engine;
	int task_thread_count;
	tyran_memory memory;
} nimbus_boot;

nimbus_boot* nimbus_boot_new();
tyran_boolean nimbus_boot_vertical_refresh(nimbus_boot* self);

#endif
