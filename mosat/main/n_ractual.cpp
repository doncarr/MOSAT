
#define _XOPEN_SOURCE /* glibc2 needs this */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rtcommon.h"
#include "arg.h"
#include "tt.h"

int n_read_actual(char *fname, time_t times[], int max, 
           time_t today, int exit_event)
{
  char line[300];

  struct tm mytm;

  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    printf("%s:%d: ",__FILE__,__LINE__);
    perror(fname);
    exit(0);
  }

  int n_lines = 0;
  time_t this_time;
  //time_t last_time;
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '\t', 6);
    if (argc == 0)
    {
      continue;
    }
    long sensor = atol(argv[0]);
    localtime_r(&today, &mytm);
    if (NULL == strptime(argv[3], "%T", &mytm))
    {
      printf("Can't convert time: %s\n", argv[3]);
      exit(0);
    }
    if (NULL == strptime(argv[2], "%F", &mytm))
    {
      printf("Can't convert date: %s\n", argv[2]);
      exit(0);
    }
    //last_time = this_time;
    this_time = mktime(&mytm);
    //if (sensor != exit_event)
    if ((sensor < 51 || sensor > 59)) 
    /* exit event numbers are between 51 and 59 */
    {
      continue;
    }
    times[n_lines] = this_time;
    n_lines++;
    if (n_lines >= max)
    {
      break;
    }
  }
  return n_lines;
}
