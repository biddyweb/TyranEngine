#ifndef tyran_object_operator_h
#define tyran_object_operator_h

#include <tyranscript/tyran_types.h>

struct tyran_value;

tyran_boolean tyran_object_operator_comparison(int comparison, struct tyran_value* a, struct tyran_value* b);

#endif
