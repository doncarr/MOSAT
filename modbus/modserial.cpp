
#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "mtype.h"
#include "modio.h"




/********************************************************************/
 
mod_serial_t::mod_serial_t(char *dev, int baud, int parity, 
		           int data_bits, int stop bits)
{
  //sock = a_socket;
  send_ptr = 0;
  recv_ptr = 0;
  total_read = 0;
  total_written = 0;
}

/********************************************************************/
 
int mod_serial_t::wait_message(void)
{
  react_trace.dprintf(0, "Waiting for a message . . . \n");
  int total_read = read(sock, recv_buffer, sizeof(recv_buffer));
  recv_buffer[total_read] = '\0';
  //printf("Got %d bytes: %s\n", total_read, recv_buffer);
  recv_ptr = 0;
  total_read = 0;
  return total_read;
}

/********************************************************************/
 
int mod_serial_t::get_buffer(uint8 *buf, int n)
{
  memcpy(buf, recv_buffer + recv_ptr, n);
  recv_ptr += n;
  return n;
}

/********************************************************************/
 
int mod_serial_t::put_buffer(uint8 *buf, int n)
{
  memcpy(send_buffer + send_ptr, buf, n);
  total_written += n;
  send_ptr += n;
  return n;
}

/********************************************************************/
 
int mod_serial_t::send_message(void)
{
  int n = write(sock, send_buffer, total_written);
  send_ptr = 0;
  total_written = 0;
  return n;
}

/********************************************************************/
 

