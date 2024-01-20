#include "../disk.h"
#include "../../core/tty/tty.h"

#include <stdint.h>

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_COMM_REGSTAT 0x1F7
#define ATA_PRIMARY_ALTSTAT_DCR  0x3F6


#define STAT_ERR  (1 << 0)
#define STAT_DRQ  (1 << 3)
#define STAT_SRV  (1 << 4)
#define STAT_DF   (1 << 5)
#define STAT_RDY  (1 << 6)
#define STAT_BSY  (1 << 7)

static inline uint8_t inb(uint16_t port) {
  uint8_t data;
  asm volatile("inb %1, %0": "=a" (data) : "Nd"(port));
  return data;
}

static inline void outb(uint16_t port, uint8_t data){
  asm volatile("outb %0, %1" :: "a" (data), "Nd" (port));
}

static inline void insw(int port, void *buffer, int count) {
  // Lire les données du port
  asm volatile(
    "insw"
    : "=a"(*(uint16_t *)buffer)
    : "d"(port)
    , "c" (count)
  );
}

void outsw(unsigned short port, const void* data, unsigned int wordCount) {
    asm volatile (
        "cld\n\t"
        "rep outsw"
        :
        : "d" (port), "S" (data), "c" (wordCount)
    );
}

uint8_t disk_get_status(disk_e disk) {
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_SECCOUNT, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_LO, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_MID, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_HI, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xEC);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);

    // Read the status port. If it's zero, the drive does not exist.
    uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
    terminal_write("waiting for status\n");
    while(status & STAT_BSY) {
        uint32_t i = 0;
        while(1) {
            i++;
        }
        for(i = 0; i < 0x0FFFFFFF; i++) {}
        terminal_write("checking regstat\n");
        status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }
    
    if(status == 0) return 0;
    terminal_write("status indicate presence of disk\n");
    while(status & STAT_BSY) {
      status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }

    uint8_t mid = inb(ATA_PRIMARY_LBA_MID);
    uint8_t hi = inb(ATA_PRIMARY_LBA_HI);
    if(mid || hi) {
        // The drive is not ATA. (Who knows what it is.)
        return 0;
    }

    // Wait for ERR or DRQ
    while(!(status & (STAT_ERR | STAT_DRQ))) {
        status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }

    if(status & STAT_ERR) {
        // There was an error on the drive. Forget about it.
        return 0;
    }

    //uint8_t *buff = kmalloc(40960, 0, NULL);
    uint8_t buff[256 * 2];
    insw(ATA_PRIMARY_DATA, buff, 256);
    // We read it!
    return 1;
}

void disk_read(disk_e disk, uint8_t* target, uint32_t LBA, uint32_t sectorcount){
  outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((LBA >> 24) & 0x0F));
  outb(ATA_PRIMARY_ERR, 0x00);
  outb(ATA_PRIMARY_SECCOUNT, sectorcount);
  outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);
  outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);
  outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);
  outb(ATA_PRIMARY_COMM_REGSTAT, 0x20);

  uint8_t i;
  for(i = 0; i < sectorcount; i++) {
    // POLL!
    while(1) {
      uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
      if(status & STAT_DRQ) {
        // Drive is ready to transfer data!
        break;
      }
    }
    // Transfer the data!
    insw(ATA_PRIMARY_DATA, (void *)target, 256);
    target += 256;
  }
}

void disk_write(disk_e disk, const uint8_t* target,uint64_t LBA, uint32_t sectorcount) {

    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x34);                   // READ SECTORS EXT

    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            }
            else if(status & STAT_ERR) {
                terminal_write("DISK SET ERROR STATUS!");
            }
        }
        // Transfer the data!
        outsw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

    // Flush the cache.
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);
    // Poll for BSY.
    while(inb(ATA_PRIMARY_COMM_REGSTAT) & STAT_BSY) {}
}
