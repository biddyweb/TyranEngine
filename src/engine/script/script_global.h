#ifndef nimbus_script_global_h
#define nimbus_script_global_h

struct tyran_mocha_api;

typedef struct nimbus_script_global {
	struct tyran_mocha_api* mocha;
} nimbus_script_global;

void nimbus_script_global_init(nimbus_script_global* self, struct tyran_mocha_api* mocha);

#endif
