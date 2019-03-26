
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rt_serial.h"
#include "simple_ascii.h"


int init_simple_ascii(const char *device, char *error, int size)
{
  int serial_fd; 
  rt_verbose = 1;
  printf("Opening device %s . . . \n", device);
  serial_fd = rt_open_serial(device ,9600,1.0);
  printf("Serial fd: %d \n", serial_fd);
  if (serial_fd < 0)
  {
    perror(device);
  }
  return serial_fd;
}

/******************************************************************/

int read_simple_ascii(int fd, 
           double *ai_vals, int max_ai, 
           bool *di_vals, int max_di, 
           bool *do_vals, int max_do, 
           char *error, int size)
{
  for (int i=0; i < max_ai; i++)
  {
    ai_vals[i] = 0.0;
  }
  for (int i=0; i < max_di; i++)
  {
    di_vals[i] = false;
  }
  for (int i=0; i < max_do; i++)
  {
    do_vals[i] = false;
  }
  //return 0;
  char buf[100];
  printf("Sending '$R' to micro\n");
  write(fd, "$R", 2);
  int ret_val = rt_read_serial(fd, buf, 5);
  buf[3] = '\0'; // We do not care about the /n/r
  printf("Got: %s\n", buf);
  if (ret_val < 0)
  {
    printf("Error reading serial port\n");
    write(fd, "XXXXX", 5);
  }
  int n = atol(buf+1); // Skip the '#'
  printf("There are %d analog values\n", n);
  for (int i=0; ((i < n) && (i < max_ai)); i++)
  {
    ret_val = rt_read_serial(fd, buf, 8);
    buf[6] = '\0'; // Get rid of the /n/r
    printf("%d AI value: got: %s\n", ret_val, buf);
    if (ret_val < 0)
    {
      printf("Error reading serial port\n");
    }
    ai_vals[i] = atof(buf+1);
  }

  ret_val = rt_read_serial(fd, buf, 19);
  buf[17] = '\0'; // Get rid of the /n/r
  printf("Got: %s\n", buf);
  if (ret_val < 0)
  {
    printf("Error reading serial port\n");
  }
  for (int i=0; (i < 16) && (i < max_di); i++)
  {
    di_vals[i] = (buf[i+1] == '1');
  }
  
  ret_val = rt_read_serial(fd, buf, 26);
  buf[24] = '\0'; // Get rid of the /n/r
  printf("Got: %s\n", buf);
  if (ret_val < 0)
  {
    printf("Error reading serial port\n");
  }
  for (int i=0; (i < 23) && (i < max_do); i++)
  {
    do_vals[i] = (buf[i+1] == '1');
  }
  return 0;
}

/******************************************************************/

int send_simple_ascii(int fd, int channel, bool val,        
           char *error, int size)
{
  char buf[20];
  snprintf(buf, sizeof(buf), "$WDO%02d%c\n\r", channel, val ? '1' : '0' );
  //snprintf(buf, sizeof(buf), "$WDO%02d%c\n\r", channel, val ? '1' : '1' );
  // $WDO011
  int ret_val = write(fd, buf, 7);
  printf("%d bytes written: %s", ret_val, buf);
  /**
  ret_val = rt_read_serial(fd, buf, 5);
  buf[5] = '\0';
  printf("%d bytes read: %s", ret_val, buf);
  buf[4] = '\0';
  if (ret_val < 0)
  {
    printf("Error reading serial port\n");
  }
  if (0 == strncasecmp(buf, "OK", 2))
  {
    return 0;
  }
  else
  {
    int err = atoi(buf);
    printf("******* Error sending do: %d\n", err);
    return err;
  }
  ***/
  return 0;
}

/******************************************************************/

int ao_send_simple_ascii(int fd, int channel, int val,        
           char *error, int size)
{
  char buf[20];
  if (val < 0) val = 0;
  if (val > 65535) val = 65535;
  snprintf(buf, sizeof(buf), "$WAO%02d%05d\n\r", channel, val);
  // $WAO0165535
  int ret_val = write(fd, buf, 11);
  printf("%d bytes written: %s", ret_val, buf);
  /**
  ret_val = rt_read_serial(fd, buf, 5);
  buf[5] = '\0';
  printf("%d bytes read: %s", ret_val, buf);
  buf[4] = '\0';
  if (ret_val < 0)
  {
    printf("Error reading serial port\n");
  }
  if (0 == strncasecmp(buf, "OK", 2))
  {
    return 0;
  }
  else
  {
    int err = atoi(buf);
    printf("******* Error sending do: %d\n", err);
    return err;
  }
  ***/
  return 0;
}

