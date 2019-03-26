
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mqueue.h>

#include "rt_serial.h"
#include "process_drv.h"


int init_process_drv(const char *device, char *error, int size)
{
  return -1;
}

/******************************************************************/

int read_process_drv(int fd, 
           double *ai_vals, int max_ai, 
           bool *di_vals, int max_di, 
           bool *do_vals, int max_do, 
           char *error, int size)
{
  return 0;
}

/******************************************************************/

int send_process_drv(int fd, int channel, bool val,        
           char *error, int size)
{
  return 0;
}

