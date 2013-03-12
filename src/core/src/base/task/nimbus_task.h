#ifndef nimbus_task_h
#define nimbus_task_h

typedef void (*nimbus_task_function)(void*);

struct nimbus_task_queue;
struct tyran_memory;

typedef struct nimbus_task {
	nimbus_task_function work;
	void* self;
	struct nimbus_task_queue* task_queue;
	int group;
	int affinity;
} nimbus_task;

nimbus_task* nimbus_task_new(struct tyran_memory* memory, nimbus_task_function work, void* self);
#endif
