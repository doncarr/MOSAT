

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dallas_1wire.h"

/* This function will return the temperature in degrees celcius as 
 * double floating point.
 */

static int fds[12];
static int n_fds = 0;

int init_dallas_wire_temperatures(char *error, int size)
{
  const char *fname = "dbfiles/Dallas1Wire.dat";
  printf("Opening %s\n", fname);
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    snprintf(error, size, "Can't open: %s", fname);
    return -1;
  }
  int fd;
  n_fds = 0;
  int max_fds = sizeof(fds) / sizeof(fds[0]);
  char line[300];
  for (int i=0; NULL != fgets(line, sizeof(line) , fp); i++)
  {
    ltrim(line);
    rtrim(line);
    if (line[0] == '#')
    {
      continue;
    }
    printf("Opening file: %s\n", line);
    fd = open(line, O_RDONLY); 
    if (-1 == fd)
    {
      perror(line);
      continue;
    }
    fds[n_fds] = fd; 
    n_fds++;
    if (n_fds >= max_fds)
    {
      break;
    } 
  }
  return n_fds;
}

/******************************************************************/

int read_dallas_wire_temperatures(double *temperatures, int max, 
           char *error, int size)
{
  char buf[100];
  for (int i=0; i < n_fds; i++)
  {
    if (fds[i] == -1)
    {
      temperatures[i] = 0.0;
      continue;
    }
    lseek(fds[i], 0, SEEK_SET);
    if (-1 == read(fds[i], buf, sizeof(buf)))
    {
      temperatures[i] = 0.0;
      perror("can't read temp");
    }
    else
    {
      temperatures[i] = atof(buf);
    }
  } 
  return n_fds;
}

/******************************************************************/
