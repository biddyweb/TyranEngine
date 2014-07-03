#ifndef tyran_symbol_h
#define tyran_symbol_h

#include <tyranscript/tyran_types.h>

typedef struct tyran_symbol {
	u32t hash;
} tyran_symbol;

#define tyran_symbol_equal(a, b) ((a)->hash == (b)->hash)

#endif
