#ifndef nimbus_engine_h
#define nimbus_engine_h

#include "../task/nimbus_task.h"
#include <tyranscript/tyran_mocha_api.h>

typedef struct nimbus_engine {
	nimbus_task task;
	tyran_mocha_api mocha_api;
} nimbus_engine;


nimbus_engine* nimbus_engine_new(tyran_memory* memory);
tyran_boolean nimbus_engine_vertical_refresh(nimbus_engine* engine);

#endif
