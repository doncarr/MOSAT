
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rt_serial.h"
#include "simple_text.h"


int init_simple_text(const char *device, char *error, int size)
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

int read_simple_text(int fd, 
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
  char buf[100];
  int ret_val;
  buf[0]='X';
  printf("Sending '$DI' to micro, fd = %d\n", fd);
  write(fd, "$DI\n", 4);
  ret_val = rt_read_serial(fd, buf, 1);
  printf("DI message: "); 
  printf("%c", buf[0]); 
  while (buf[0] != '#')
  {
    if (ret_val <= 0)
    {
      perror("rt_read_serial");
      printf("Error reading serial port\n");
      return 0;
    }
    ret_val = rt_read_serial(fd, buf, 1);
    printf("%c", buf[0]); 
  }
  int n_di = 0;
  ret_val = rt_read_serial(fd, buf, 1);
  while (buf[0] != '\n')
  {
    if (ret_val < 0)
    {
      printf("Error reading serial port\n");
      return 0;
    }
    else if ((buf[0] != '0') && (buf[0] != '1'))
    {
      printf("Invalid character in input: %c\n", buf[0]);
      return 0;
    }
    printf("%c", buf[0]); 
    di_vals[n_di] = buf[0] == '1';
    ret_val = rt_read_serial(fd, buf, 1);
    n_di++;
  }
  printf("\n"); 
  return 0;
}

/******************************************************************/

int send_simple_text(int fd, int channel, bool val,        
           char *error, int size)
{
  char buf[10];
  snprintf(buf, sizeof(buf), "$DO%02d%c\n", channel, val ? '1' : '0' );
  int ret_val = write(fd, buf, 7);
  printf("%d bytes written: %s", ret_val, buf);
  ret_val = rt_read_serial(fd, buf, 4);
  buf[4] = '\0';
  printf("%d bytes read: %s", ret_val, buf);
  buf[4] = '\0';
  if (ret_val < 0)
  {
    printf("Error reading serial port\n");
  }
  if (0 == strncasecmp(buf, "#OK", 3))
  {
    return 0;
  }
  else
  {
    int err = atoi(buf+1);
    printf("******* Error sending do: %d\n", err);
    return err;
  }
  return 0;
}

