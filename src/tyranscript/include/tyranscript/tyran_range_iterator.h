#ifndef tyran_range_iterator_h
#define tyran_range_iterator_h

struct tyran_memory;

typedef struct tyran_range_iterator {
	int current;
	int max;
	int delta;
} tyran_range_iterator;

tyran_range_iterator* tyran_range_iterator_new(struct tyran_memory* memory, int start, int end);
int* tyran_range_iterator_next(tyran_range_iterator* iterator);
void tyran_range_iterator_free(tyran_range_iterator* self);

#endif
