#ifndef nimbus_event_execute_function_h
#define nimbus_event_execute_function_h

#include <tyran_engine/combine/combine_instance.h>
#include <tyranscript/tyran_symbol.h>

extern const u8t NIMBUS_EVENT_EXECUTE_FUNCTION;

struct nimbus_event_write_stream;

typedef struct nimbus_event_execute_function {
	nimbus_combine_instance_id instance_id;
	tyran_symbol function_symbol;
} nimbus_event_execute_function;

void nimbus_event_execute_function_send(struct nimbus_event_write_stream* stream, nimbus_combine_instance_id id, tyran_symbol function_symbol);

#endif
