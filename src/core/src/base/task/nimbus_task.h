#ifndef nimbus_task_h
#define nimbus_task_h

typedef void (*number_task_function)(void*);

struct nimubs_task_queue;

typedef struct nimbus_task {
	number_task_function work;
	void* self;
	struct nimbus_task_queue* task_queue;
	int group;
	int affinity;
} nimbus_task;

#endif
