#ifndef STDLIB_STRING_H
#define STDLIB_STRING_H
#include <stddef.h>
void *memcpy(void *__restrict dest, const void *__restrict src, size_t n);
void *memset(void* pointer, int value, size_t count);
unsigned int strlen(char* c);
int strcmp(const char*,const char*);
#endif
