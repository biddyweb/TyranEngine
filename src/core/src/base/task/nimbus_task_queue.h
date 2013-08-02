#ifndef nimbus_task_queue_h
#define nimbus_task_queue_h

#include "../mutex/nimbus_mutex.h"
#include "tyranscript/tyran_types.h"

struct nimbus_task;

typedef struct nimbus_task_queue {
	int group_counter[8];
	nimbus_mutex mutex;
	struct nimbus_task* tasks[128];
	int task_count;
	int task_write_index;
	int task_read_index;
	int task_max_count;
} nimbus_task_queue;

struct tyran_memory;

nimbus_task_queue* nimbus_task_queue_new(struct tyran_memory* memory);
void nimbus_task_queue_destroy(nimbus_task_queue* task_queue);
void nimbus_task_queue_add_task(nimbus_task_queue* task_queue, struct nimbus_task* task);
tyran_boolean nimbus_task_queue_has_pending_tasks_from_group(nimbus_task_queue* task_queue, int group);
struct nimbus_task* nimbus_task_queue_fetch_next_task(nimbus_task_queue* task_queue, int requested_affinity);
void nimbus_task_queue_task_completed(nimbus_task_queue* task_queue, struct nimbus_task* task);
struct nimbus_task* nimbus_task_queue_fetch_next_task_from_affinity(nimbus_task_queue* self, int requested_affinity);
#endif
