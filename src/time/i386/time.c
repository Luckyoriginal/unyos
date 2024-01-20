#include "../time.h" 

void time_delay(long delay){
  int start;
  int end;
  asm volatile( "rdtsc" : "=A" (start));

  do {
    asm volatile("rdtsc" : "=A" (end));
  }while (end-start<delay);
}

long time_get(){
  int time;
  asm volatile( "rdtsc" : "=A" (time));
  return time;
}
