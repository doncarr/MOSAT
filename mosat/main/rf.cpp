
#define _XOPEN_SOURCE /* glibc2 needs this */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rtcommon.h"
#include "arg.h"
#include "tt.h"

int read_schedule(char *fname, int train_num[], time_t times[], int max, time_t today)
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
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '\t', 2);
    if (argc == 0)
    {
      continue;
    }
    long train = atol(argv[0]);
//char *strptime(const char *s, const char *format, struct tm *tm);
    //time_t mytime;
    //mytime = time(NULL);
    localtime_r(&today, &mytm);
    if (NULL == strptime(argv[1], "%T", &mytm))
    {
      printf("%s:%d: ",__FILE__,__LINE__);
      perror(argv[1]);
      exit(0);
    }


    times[n_lines] = mktime(&mytm);
    train_num[n_lines] = train;    
    n_lines++;
    if (n_lines >= max)
    {
      break;
    }
  }
  return n_lines;
}

/**
int main(int xargc, char *xargv[])
{
  int train_num[500];
  time_t scheduled[500];
  int n = read_schedule("day.txt", train_num, scheduled, 500);
  for (int i=0; i < n; i++)
  {
    char buf1[30];
    struct tm mytm;
    localtime_r(&scheduled[i], &mytm);
    strftime(buf1, sizeof(buf1), "%T, %F", &mytm);
    printf("%02ld %s\n", train_num[i], buf1); 
  }
}
***/
