
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "arg.h"
#include "rtcommon.h"
#include "evlog.h"

/******************************************************/

evlog_t::evlog_t(char *the_dir)
{
  fp1 = NULL;
  fp2 = NULL;
  theday = -1;
  local_dir = strdup(the_dir);
}

/******************************************************/

void evlog_t::reopen_files(void)
{
  if (fp1 != NULL) fclose(fp1);
  if (fp2 != NULL) fclose(fp2);
  this->open_files();
}

/******************************************************/

void evlog_t::check_day(void)
{
  if (this->get_day() != theday)
  {
    this->reopen_files();
  }
}

/******************************************************/

static long get_last_seq_num(char *fname)
{
  printf("getting last seq_num from %s\n", fname);
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    return -1;
  }
  long last_seq_num = -1;
  long this_seq_num = -1;
  char line[300];
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '\t', 20);
    if (argc == 0)
    {
      continue;
    }
    //for(int i=0; i < argc; i ++) printf("%d:%s\t", i, argv[i]);
    //printf("\n");
    this_seq_num = atol(argv[6]);
    if (this_seq_num > 0)
    {
      last_seq_num = this_seq_num;
    }
    //printf("seq num: %ld\n", last_seq_num);
  }
  return last_seq_num;
}

/******************************************************/

long evlog_t::open_files(void)
{
  return this->open_files(true);
}

/******************************************************/

void evlog_t::only_open_files(void)
{
  this->open_files(false);
}

/******************************************************/

long evlog_t::open_files(bool find_seq)
{
  char fname1[200];
  file_name("_event.txt", fname1, sizeof(fname1));
  fp1 = fopen(fname1, "a");
  theday = this->get_day();
  if (fp1 == NULL)
  {
    perror(fname1);
    exit(1);
  }
  char fname2[200];
  file_name("_log.txt", fname2, sizeof(fname2));
  long last_seq_num = -1;
  if (find_seq) 
  {
    last_seq_num = get_last_seq_num(fname2);
  }
  fp2 = fopen(fname2, "a");
  if (fp2 == NULL)
  {
    perror(fname2);
    exit(1);
  }
  return last_seq_num;
}

/******************************************************/

int evlog_t::get_day(void)
{
  time_t this_time;
  this_time = time(NULL);
  struct tm mytm;
  localtime_r(&this_time, &mytm);
  return mytm.tm_mday;
}

/******************************************************/

void evlog_t::file_name(const char *post, char *buf, int sz)
{
  time_t this_time;
  this_time = time(NULL);
  struct tm mytm;
  localtime_r(&this_time, &mytm);
  char year[10];
  char month[10];
  char day[10];
  snprintf(year, sizeof(year), "%d", mytm.tm_year + 1900);
  snprintf(month, sizeof(month), "%02d", mytm.tm_mon + 1);
  snprintf(day, sizeof(day), "%02d", mytm.tm_mday);
  //snprintf(buf, sz, "/home/react/public_html/log/%s%s%s%s", year, month, day, post);
  //snprintf(buf, sz, "../public_html/log/%s%s%s%s", year, month, day, post);
  snprintf(buf, sz, "%s/%s%s%s%s", this->local_dir, year, month, day, post);
}

/******************************************************/

void evlog_t::printf(const char *fmt, ...)
{
  if (fp1 == NULL) return;
  this->check_day();
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  vfprintf(fp1, fmt, arg_ptr);
  fflush(fp1);
}
			  
/******************************************************/

void evlog_t::print_event(long the_id, const char *str, time_t mytime)
{
  if (fp1 == NULL) return;
  this->check_day();
  char buf[100];
  struct tm mytm;
  localtime_r(&mytime, &mytm);
  strftime(buf, sizeof(buf), "%F   %T", &mytm);
  fprintf(fp1, "%ld %s : %s\n", the_id, buf, str);
  fflush(fp1);
}

/******************************************************/

void evlog_t::print_log(long the_id, const char *tp, time_t start_time, time_t end_time, 
		int n_counts, int start_count, int end_count)
{
  if (fp2 == NULL) return;
  this->check_day();
  char buf[100];
  struct tm mytm;
  fprintf(fp2, "%ld\t%s\t", the_id, tp);
  localtime_r(&start_time, &mytm);
  strftime(buf, sizeof(buf), "%F\t%T", &mytm);
  fprintf(fp2, "%s\t", buf);
  localtime_r(&end_time, &mytm);
  strftime(buf, sizeof(buf), "%T", &mytm);
  fprintf(fp2, "%s\t%d\t%d\t%d\n", buf, n_counts, start_count, end_count);
  fflush(fp2);
}

/******************************************************/


