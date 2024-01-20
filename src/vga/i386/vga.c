#include "../vga.h" 
#include <stddef.h>
#include <stdint.h>

uint16_t* VGA_MEMORY;
size_t VGA_WIDTH;
size_t VGA_HEIGHT;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void vga_init(){
  VGA_WIDTH = 80;
  VGA_HEIGHT = 25;
  VGA_MEMORY = (uint16_t*)0xB8000;
}

void vga_put(char c,uint16_t x, uint16_t y, enum vga_color fontcolor, enum vga_color  backcolor){
  VGA_MEMORY[y*VGA_WIDTH + x ]= vga_entry(c, vga_entry_color(fontcolor, backcolor));
}

char vga_get_char(uint16_t x, uint16_t y){
  return VGA_MEMORY[y*VGA_WIDTH + x];
}
