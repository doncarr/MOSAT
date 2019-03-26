#include <stdint.h>

unsigned char read_di(int n);
unsigned char read_do(int n);
void send_do(int n, unsigned char val);
uint16_t read_ai(int n);
uint16_t read_register(int n);
void write_register(int n, uint16_t val);

unsigned char check_range_di(int start, int end);
unsigned char check_range_do(int start, int end);
unsigned char check_range_ai(int start, int end);
unsigned char check_range_reg_in(int start, int end);
unsigned char check_range_reg_out(int start, int end);

