#include "../vga/vga.h"
#include "tty/tty.h"
#include "../disk/disk.h"
#include "../time/time.h"
#include "../keyboard/keyboard.h"
#include "../memory/memory.h"

void k_main(void ){

    unsigned int ecx = 0x0010;
    unsigned int eax, ebx;

    // Effectuer la lecture du registre MSR dans les registres edx:eax
    asm volatile (
        "rdmsr"
        : "=a" (eax), "=d" (ebx)
        : "c" (ecx)
    );

  memory_init(); //initialisation de la memoire
  terminal_initialize();// inittialisation du terminal
  int* x = memory_malloc(sizeof(int)*32);//test de l'alocateur
  terminal_write("hello world\n");
  for(int i=0;i<32;i++){
    x[i]=i;
    terminal_write_int(x[i]);
  }
    terminal_write("init disk\n");
  disk_get_status();
  long start = time_get();
  time_delay(1000000000);
  terminal_write("done");
  memory_free(x, sizeof(int)*32);
  terminal_write_int(eax);
  return;
}
