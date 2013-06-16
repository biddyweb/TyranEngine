#ifndef nimbus_task_h
#define nimbus_task_h

struct nimbus_task_queue;

typedef void (*nimbus_task_function)(void*, struct nimbus_task_queue* queue);

struct tyran_memory;

typedef struct nimbus_task {
	nimbus_task_function work;
	void* self;
	struct nimbus_task_queue* task_queue;
	int group;
	int affinity;
} nimbus_task;

void nimbus_task_init(nimbus_task* self, nimbus_task_function work, void* task_self);
void nimbus_task_call(nimbus_task* self, struct nimbus_task_queue* task_queue);

#endif
