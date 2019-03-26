
#ifndef __HPRTU_INC__
#define __HPRTU_INC__

#include "rtmodbus.h"

void add_CRC(uint8 *buf, int size, uint16 start_mask);
int check_CRC(uint8 *buf, int size, uint16 start_mask);
uint16 CRC_16(uint8 *buf, int n);
void swap16(uint16 *word);

int hexascii_to_binary(uint8 bin_buf[], uint8 hex_buf[], int n_hex);
int binary_to_hexascii(uint8 hex_buf[], uint8 bin_buf[], int n_bin);
void print_buf(char *prefix, uint8 *bin_buf, int cnt);

struct pc_com_info_t
{
  long baud_rate;
  int com;
  int base_address;
  int irq;
  int data_bits;
  int stop_bits;
  int flow_control;
  int timeout;
  char parity;
};

struct request_t
{
  uint8 hp_type;
  uint8 iom;
  uint8 channel;
  uint8 bit;
  uint8 mod_type;
  uint16 mod_number;
  uint16 data;
};

void hp_req_sort(request_t *req, int n);
void mod_req_sort(request_t *req, int n);

struct channel_t
{
  uint8 hp_type;
  uint8 decimals;
  uint32 data;
  uint8 mod_type;
  uint16 mod_number;
};

struct iom_t
{
  channel_t channel[32];
};

struct hp_rtu_t
{
  uint16 rtu_number;
  iom_t iom[8];
};

struct debug_info_t
{
  int debug_level;
  int save_to_file;
  int modem_on;
};

class MODSerial : public rtmodbus_t
{
private:
  //int retries;
  double timeout;
  mod_io_t *modio;
  uint8 address;
public:
  MODSerial(mod_io_t *an_io) {address = 2; modio = an_io;};
  int reply_size(uint8 *buf);
  int read_di(uint16 start_address, int n, bool *vals);
  int read_di_register(unsigned short start_register, int n_di, bool *vals);
  int read_do(uint16 start_address, int n, bool *vals);
  int read_ai(uint16 start_address, int n, uint16 *vals);
  int read_reg(uint16 start_address, int n, uint16 *vals);
  int read_di_req(request_t *req);
  int read_do_req(request_t *req);
  //int read_ai_req(request_t *req);
  int read_reg_req(request_t *req);
  int write_reg(uint16 num, uint16 val);
  int send_do(uint16 num, uint8 val);
  int write_multiple_regs(uint16 num, int n, uint16 *vals);
  int send_multiple_dos(uint16 num, int n, uint8 *vals);
  void set_address(uint8 an_addr) 
        {printf("setting address to %u\n", an_addr); address = an_addr;};
  uint8 get_address(void) {return address;};
  void set_timeout(double a_timeout) {timeout = a_timeout;};
  void set_debug_level(int a_debug_level);
  int send(uint8 *buf, int size);
  int receive(uint8 *buf, int size);
};

#endif
