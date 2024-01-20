#include "tty.h" 
#include "../../vga/vga.h"
#include <stddef.h>
#include <stdint.h>

static uint16_t terminal_color;
static uint16_t font_color;

extern size_t VGA_WIDTH;
extern size_t VGA_HEIGHT;
extern uint16_t* VGA_MEMORY;

static size_t terminal_row = 0;
static size_t terminal_collumn = 0;

void terminal_initialize(){
  vga_init();
  terminal_color = VGA_COLOR_BLACK;
  font_color = VGA_COLOR_LIGHT_GREY;
  //clear screen:
  for(size_t y=0; y<VGA_HEIGHT;y++){
    for(size_t x=0;x<VGA_WIDTH;x++){
      vga_put(' ',x,y, font_color,terminal_color);
    }
  }
}

void terminal_scroll(unsigned int line){
  for(size_t y=0;y<VGA_HEIGHT - line;y++){
    for(size_t x=0;x<VGA_WIDTH;x++){
      vga_put(vga_get_char(x , y+line), x , y, font_color, terminal_color);
    }
  }
}

void terminal_putchar(char c){
  int line;
  if (c!='\n'){
    vga_put(c,terminal_collumn,terminal_row,font_color,terminal_color);
 
  if (++terminal_collumn == VGA_WIDTH){
    terminal_collumn = 0;
    if (++terminal_row == VGA_HEIGHT){
      for(line = 1; (size_t)line <=VGA_HEIGHT-1; line++){
        terminal_scroll(line);
      }
    }
  }
 }else{
    terminal_collumn = 0;
    if (++terminal_row == VGA_HEIGHT){
      for(line = 1; (size_t)line <=VGA_HEIGHT-1; line++){
        terminal_scroll(line);
      }
    }
  }
}

void terminal_write(const char *data){
  int cursor =0;
  while(data[cursor]!='\0'){
    terminal_putchar(data[cursor]);
    cursor++;
  }
}

void terminal_write_int(int x){
  if (x<0){
    terminal_putchar('-');
    x=-x;
  }
  char data[12];
  int pw = 1;
  int max = 0;
  for (int i=0;i<12;i++){
    data[i] = '0'+(x/pw)%10;
    pw*=10;
    if (data[i]!='0'){
      max = i;
    }
  }
  for (int i=11-max;i<12;i++){
    terminal_putchar(data[11-i]);
  }
}
