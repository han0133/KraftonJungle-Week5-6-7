#ifndef MM_BLOCK_H
#define MM_BLOCK_H

#include <stddef.h>
#include <stdbool.h>

size_t get_size(void *header_ptr);
bool is_allocated(void *header_ptr);
void *get_header_pointer(void *payload_ptr);
void *get_footer_pointer(void *payload_ptr);
void *get_next_payload_pointer(void *payload_ptr);
void *get_prev_payload_pointer(void *payload_ptr);

#endif /* MM_BLOCK_H */