

#include <stdio.h>
#include <stdint.h>

#include "mod_server_msg.h"
#include "rt_modbus_crc.h"


void doit(uint8_t buf[])
{
  int min_size;
  int total_size;
  int reply_size;
  uint8_t ascii[1024];

  min_size = rt_modbus_min_bytes(buf);
  printf("Min size = %d\n", min_size);
  total_size = rt_modbus_total_bytes(buf, min_size);
  printf("Total size = %d\n", total_size);

  if (check_CRC(buf, total_size, 0xFFFF))
  {
     printf("CRC matches\n");
  }
  else
  {
     printf("*** BAD CRC\n");
  }

  binary_to_hexascii(ascii, buf, total_size);
  printf("Message: %s\n", ascii);

  reply_size = rt_modbus_process_request(buf, total_size);
  printf("Reply size = %d\n", reply_size);

  binary_to_hexascii(ascii, buf, reply_size);
  printf("Reply: %s\n", ascii);

  if (check_CRC(buf, reply_size, 0xFFFF))
  {
     printf("CRC matches\n");
  }
  else
  {
     printf("*** BAD CRC\n");
  }
}

/*********/

int main(int argc, char *argv[])
{
  uint8_t buf[512];

  buf[0] = 0x05;
  buf[1] = 0x01;
  buf[2] = 0x00;
  buf[3] = 0x99;
  buf[4] = 0x00;
  buf[5] = 0x55;
  add_CRC(buf, 8, 0xFFFF);

  printf("\nExample 0x01 ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x02;
  buf[2] = 0x00;
  buf[3] = 0xC4;
  buf[4] = 0x00;
  buf[5] = 0x16;
  add_CRC(buf, 8, 0xFFFF);

  printf("\nExample 0x02 ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x03;
  buf[2] = 0x00;
  buf[3] = 0x10;
  buf[4] = 0x00;
  buf[5] = 0x05;
  add_CRC(buf, 8, 0xFFFF);

  printf("\nExample 0x03 ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x04;
  buf[2] = 0x00;
  buf[3] = 0x12;
  buf[4] = 0x00;
  buf[5] = 0x06;
  add_CRC(buf, 8, 0xFFFF);

  printf("\nExample 0x04 ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x05;
  buf[2] = 0x00;
  buf[3] = 0xAC;
  buf[4] = 0xFF;
  buf[5] = 0x00;
  add_CRC(buf, 8, 0xFFFF);

  printf("\nExample 0x05 ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x06;
  buf[2] = 0x00;
  buf[3] = 0x01;
  buf[4] = 0x00;
  buf[5] = 0x03;
  add_CRC(buf, 8, 0xFFFF);

  printf("\nExample 0x06 ---------------------\n\n");
  doit(buf);


  buf[0] = 0x05;
  buf[1] = 0x0F;
  buf[2] = 0x00;
  buf[3] = 0x13;
  buf[4] = 0x00;
  buf[5] = 0x0A;
  buf[6] = 0x02;
  buf[7] = 0x55;
  buf[8] = 0x55;
  add_CRC(buf, 11, 0xFFFF);

  printf("\nExample 0x0F ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x10;
  buf[2] = 0x00;
  buf[3] = 0x01;
  buf[4] = 0x00;
  buf[5] = 0x02;
  buf[6] = 0x04;
  buf[7] = 0x0A;
  buf[8] = 0x0B;
  buf[9] = 0x01;
  buf[10] = 0x02;
  add_CRC(buf, 13, 0xFFFF);

  printf("\nExample 0x10 ---------------------\n\n");
  doit(buf);

  buf[0] = 0x05;
  buf[1] = 0x0A; // bad opcode
  buf[2] = 0x00;
  buf[3] = 0x01;
  buf[4] = 0x00;
  buf[5] = 0x03;
  add_CRC(buf, 8, 0xFFFF);

  // ******* Special note
  // For as many known opcodes as possible, we should at least
  // determine the size, read the whole message check the CRC, 
  // and return "opcode not supported".
  //
  // If it is a completely unknown opcode, then we need to wait a 
  // timeout period, flush the input buffer, then return the
  // appropriate error.

  printf("\nExample 0x0A bad opcode  ---------------------\n\n");
  doit(buf);

  return 0;
}
