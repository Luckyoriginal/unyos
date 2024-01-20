#include "string.h"
#include <stddef.h>

void *memcpy(void* dest, const void* src, size_t size){
  char* cdest = dest;
  const char* csrc = src;
  
  size_t i;
  for(i =0;i<size;i++){
    cdest[i] = csrc[i];
  }
  return dest;
}

void* memset(void *pointer, int value, size_t count){
   for(unsigned int i=0; i<count;i++){
    ((char*)pointer)[i]=value;
  }
  return pointer;
}

unsigned int strlen(char *c){
  int count = 0;
  while(c[count]!='\0'){
    count++;
  }
  return count;
}

int strcmp(const char* s1,const char* s2){
    while(*s1 && *s2) {
        if(*s1 != *s2) {
            return s1 - s2;
        }
        s1++;
        s2++;
    }
    if(*s1) return 1;
    if(*s2) return -1;
    return 0;
}
