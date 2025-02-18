#ifndef ACTRALLOC_H
#define ACTRALLOC_H

extern void actr_free(void *ptr);
extern void * actr_malloc(int size);

/// @brief returns a simple usage memory report
/// @return allocated string must be freed
extern char *actr_memory_report();
extern long long actr_memory_usage();

#endif
