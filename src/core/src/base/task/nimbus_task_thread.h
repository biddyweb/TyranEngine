#ifndef nimbus_task_thread_h
#define nimbus_task_thread_h

#include "../thread/nimbus_thread.h"

struct nimbus_task_queue;
struct tyran_memory;

typedef struct nimbus_task_thread {
	struct nimbus_task_queue* task_queue;
	nimbus_thread* thread;
	int affinity;
} nimbus_task_thread;

nimbus_task_thread* nimbus_task_thread_new(struct tyran_memory* memory, struct nimbus_task_queue* task_queue, int affinity);
void nimbus_task_thread_work(nimbus_task_thread* task_thread);

#endif
