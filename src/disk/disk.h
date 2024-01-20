#ifndef _DISK_H
#define _DISK_H 
#include <stdint.h>

typedef enum {
  HARD_DISK
}disk_e;

uint8_t disk_get_status(disk_e disk);
//void disk_read(uint64_t LBA, uint16_t sectorCount,uint8_t* target);
void disk_read(disk_e disk, uint8_t* target, uint32_t LBA, uint32_t sectorcount);
void disk_write(disk_e disk, const uint8_t* target,uint64_t LBA, uint32_t sectorcount);
#endif
