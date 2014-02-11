#ifndef tyran_runtime_debug_h
#define tyran_runtime_debug_h

struct tyran_runtime;
struct tyran_object;

void tyran_runtime_debug_who_is_referencing(const struct tyran_runtime* runtime, const struct tyran_object* o);

#endif
