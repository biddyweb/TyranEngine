#ifndef nimbus_script_global_h
#define nimbus_script_global_h

struct tyran_mocha_api;
struct tyran_value;

typedef struct nimbus_script_global {
	struct tyran_mocha_api* mocha;
	struct tyran_object* context;
} nimbus_script_global;

void nimbus_script_global_init(nimbus_script_global* self, struct tyran_mocha_api* mocha);

#endif
