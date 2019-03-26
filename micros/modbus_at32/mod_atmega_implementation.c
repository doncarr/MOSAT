#include "mod_atmega_implementation.h"

unsigned char  check_range_di(int start, int end) { return 1; }
unsigned char  check_range_do(int start, int end) { return 1; }
unsigned char  check_range_ai(int start, int end) { return 1; }
unsigned char  check_range_reg_in(int start, int end) { return 1; }
unsigned char  check_range_reg_out(int start, int end) { return 1; }

unsigned char  read_di(int n) { return ((n % 2) == 0); }
unsigned char  read_do(int n) { return ((n % 3) != 0); }
void send_do(int n, unsigned char  val) 
{  
  //printf("value: %c, addr: %d\n", (val != 0)?'T':'F', n); 
}
//void send_do(int n, unsigned char val);

uint16_t read_ai(int n) { return ((n % 2) == 0) ? 0xaaaa : 0xbbbb; }
uint16_t read_register(int n) { return ((n % 2) == 0) ? 0xffff : 0x5555; }
void write_register(int n, uint16_t val) 
{ 
  //printf("*************** Write Register value: %hx, addr: %d\n", val, n); 
}