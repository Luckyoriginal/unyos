#include "memory.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#define MAX_MEMORY 1600000000 //200 Mb
extern uint32_t end_of_kernel;
void* placement_address;
void* searching_address;
bool was_full = false;

typedef struct free_block free_block;

struct free_block{
  void* addr;
  bool free;
  free_block* next;
};


void memory_init(void){
  placement_address = &end_of_kernel + 0x1000;
  searching_address = (void*)(MAX_MEMORY+placement_address);
}

/*static uint32_t kmalloc_impl(uint32_t sz, int align, uint32_t* phys){
  if (align ==1 && (placement_address & 0xFFFFF000)){
    //if the address is not already page-aligned aligned it 
    placement_address &=0xFFFFF000;
    placement_address += 0x1000;
  }
  if (phys){
    *phys = placement_address;
  }
  uint32_t tmp = placement_address;
  placement_address += sz;
  return tmp;
}*/

typedef struct bloc{
  bool free;
  uint16_t size;
}bloc;

static void* search_free(size_t size){
  void* address = searching_address - sizeof(bloc);
  bloc cur = *(bloc*)(address);

  while(cur.size<size || cur.free == false){
    address+=cur.size;
  }
  return NULL;
}

void* memory_malloc(size_t size){
  void* ret = placement_address;
  if (placement_address + sizeof(bloc)>MAX_MEMORY){
    ret = search_free(size);  
  }
  *(bloc*)ret = (bloc){.free=false,.size = (uint32_t)size};
  ret+=sizeof(bloc);
  placement_address+=size+sizeof(bloc);  
  
  return ret;
}

void memory_free(void *address, size_t size){
  if (address<searching_address){
    searching_address = address;
  }
  bloc mem = *(bloc*)(address - sizeof(bloc));
  mem.free = true;
}
