#ifndef MM_UTILS_H
#define MM_UTILS_H

#include <stddef.h>
#include <stdint.h>

size_t align_size(size_t size);
void update_value(void *ptr, unsigned long val);
unsigned long get_value(void *ptr);
void debug();

#endif