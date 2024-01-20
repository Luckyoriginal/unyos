#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>
#include <stddef.h>
void memory_init();
void* memory_malloc(size_t size);
void memory_free(void* address,size_t size);
#endif
