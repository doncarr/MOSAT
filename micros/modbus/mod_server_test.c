
#include <stdio.h>

#include <stdint.h>

#include "mod_local_implementation.h"

unsigned char  check_range_di(int start, int end) { return 1; }
unsigned char  check_range_do(int start, int end) { return 1; }
unsigned char  check_range_ai(int start, int end) { return 1; }
unsigned char  check_range_reg_in(int start, int end) { return 1; }
unsigned char  check_range_reg_out(int start, int end) { return 1; }

unsigned char  read_di(int n) 
{ 
  unsigned char val;
  val = ((n % 2) == 0); 
  printf("DI %d, return %c\n", n, val ? 'T' : 'F');
  return val; 
}

unsigned char  read_do(int n) 
{ 
  unsigned char val;
  val = ((n % 3) != 0); 
  printf("DO %d, return %c\n", n, val ? 'T' : 'F');
  return val; 
}

void send_do(int n, unsigned char  val) 
{  
  printf("value: %c, addr: %d\n", (val != 0)?'T':'F', n); 
}
//void send_do(int n, unsigned char val);

uint16_t read_ai(int n) { return ((n % 2) == 0) ? 0xaaaa : 0xbbbb; }
uint16_t read_register(int n) { return ((n % 2) == 0) ? 0xffff : 0x5555; }
void write_register(int n, uint16_t val) 
{ 
  printf("*************** Write Register value: %hx, addr: %d\n", val, n); 
}

