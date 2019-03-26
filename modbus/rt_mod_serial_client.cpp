
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rt_serial.h"
#include "rtmodbus_utility.h"
#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "trace.h"

/**
#include "mtype.h"
#include "modio.h"
#include "evlog.h"
#include "ppc_logger.h"

***/


/****
class rt_mod_serial_client_t : public mod_io_t
{
private:
  int serial_fd;
  int baud_rate;
  float timeout;
  uint8 recv_buffer[1024];
  uint8 send_buffer[1024];

  int send_ptr;
  int recv_ptr;
  int total_read;
  int total_written;
public:
  rt_mod_serial_client_t(char *dev_name, int baudrate, float timeout);
*********/

/***************/

int rt_mod_serial_client_t::wait_message(void)
{
  recv_ptr = 0;
  react_trace.dprintf(0, "Waiting for a message . . . \n");
  // Read the first 2 bytes, all modbus replies must have a minimum of 3 bytes.
  printf("--------------- Waiting message . . .\n");
  int n1 = rt_read_serial(serial_fd, recv_buffer, 2);
  printf("--------------- Read %d bytes\n", n1);
  if (n1 != 2)
  {
    react_trace.dperror(6, "read");
  }
  int min_mod_size = rt_modbus_reply_size_min(recv_buffer);
  printf("--------------- min size: %d bytes\n", min_mod_size);
  //int mod_size = 10; 
  uint8 tmp8;
  memcpy(&tmp8, recv_buffer + 0, 1);
  react_trace.dprintf(0, "unit id = %d, ", (int) tmp8);
  // Read the rest of the buffer.
  int n = rt_read_serial(serial_fd, recv_buffer + 2, min_mod_size - 2);

  if (n != min_mod_size - 2)
  {
    react_trace.dprintf(5, "**** Wrong number of bytes in PLC reply\n");
    //RXFlush();
    return -1;
  }

  int total_mod_size = rt_modbus_reply_size_total(recv_buffer);
  printf("--------------- total size: %d bytes\n", total_mod_size);
  if (total_mod_size > min_mod_size)
  {
    printf("--------------- reading %d more bytes\n", 
                   total_mod_size - min_mod_size);
    n = rt_read_serial(serial_fd, recv_buffer + min_mod_size, total_mod_size - min_mod_size);

    if (n != total_mod_size - min_mod_size)
    {
      react_trace.dprintf(5, "**** Wrong number of bytes in PLC reply\n");
      //RXFlush();
      return -1;
    }
  }
  else
  {
    printf("--------------- no more bytes to read!\n");
  }

  if (!check_CRC(recv_buffer, total_mod_size, 0xffff))
  {
    react_trace.dprintf(5, "**** Bad CRC in message from PLC\n");
    //RXFlush();
    return -1;
  }

  react_trace.print_buf(0, "Recived:\n", recv_buffer, total_mod_size);
  return total_mod_size - 2; // Important to subtract 2, we only check
                             // the CRC, but receiving module knows 
                             // nothing about CRC.
}

/***************/

int rt_mod_serial_client_t::get_buffer(uint8 *buf, int n)
{
  memcpy(buf, recv_buffer + recv_ptr, n);
  recv_ptr += n;
  return n;
}

/***************/

int rt_mod_serial_client_t::put_buffer(uint8 *buf, int n)
{
  memcpy(send_buffer + send_ptr, buf, n);
  total_written += n;
  send_ptr += n;
  return n;
}

/***************/

int rt_mod_serial_client_t::send_message(void)
{
  add_CRC(send_buffer, total_written + 2, 0xffff);
  react_trace.print_buf(0, "\nSending to PLC ....\n", send_buffer, total_written+2);
  printf("++++++++++++ sending: %d bytes\n", total_written+2);
  int n = write(serial_fd, send_buffer, total_written+2);  
  printf("++++++++++++ sent: %d bytes\n", n);
  send_ptr = 0;
  total_written = 0;
  return n-2;
}

/***************/

rt_mod_serial_client_t::rt_mod_serial_client_t(const char *a_dev_name, int a_baudrate, float a_timeout)
{
  snprintf(device_name, sizeof(device_name), "%s", a_dev_name);
  baudrate = a_baudrate;
  timeout = a_timeout;
  rt_verbose = 1;
  /* open device */
  printf("Opening device %s, %d, %0.1f. . . ", device_name, baudrate, timeout);
  serial_fd = rt_open_serial(device_name, baudrate, timeout);
  if (serial_fd == -1)
  {
    exit(0);
  }
  printf("Device opened: %d \n", serial_fd);
  send_ptr = 0;
  total_written = 0;
  total_read = 0; 
}

/***************/

