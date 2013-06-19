#ifndef nimbus_task_thread_h
#define nimbus_task_thread_h

#include "../thread/nimbus_thread.h"

struct nimbus_task_queue;
struct tyran_memory;

typedef struct nimbus_task_thread {
	struct nimbus_task_queue* task_queue;
	nimbus_thread thread;
	int affinity;
} nimbus_task_thread;

void nimbus_task_thread_init(nimbus_task_thread* self, struct nimbus_task_queue* task_queue, int affinity);

#endif
