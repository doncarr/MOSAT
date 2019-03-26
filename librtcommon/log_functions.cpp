
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**********************************************************************/

time_t rt_calc_next_sample_time(time_t now, int sample_interval)
{
  /* This function calculates the next sample at an even number of time intervals from the
     top of the hour. This is used so that, for instance, it you want samples everh 10
     minutes, they happen at the top of the hour, 10 minutes after the hour, etc. 
     Without this calculation, you could have 2 minutes 10 seconds after the hour,
     12 minuted 10 seconds after the hour, etc. 
  */

  struct tm nowtm;
  long next_secs = 0;

  localtime_r(&now, &nowtm);
  printf("%s%ld secs since the Epoch\n", asctime(&nowtm), now);
  printf("minutes: %d, seconds: %d \n", nowtm.tm_min, nowtm.tm_sec);
  int secs_after_hour = nowtm.tm_sec + (nowtm.tm_min * 60);
  printf("Secs after hour: %d\n", secs_after_hour);

  for (int i=0;
       ((i * sample_interval) < (secs_after_hour)) && (i < 500); i++)
  {
    printf("%d: %d\n", i, i * sample_interval);
    next_secs = (i + 1) * sample_interval;
    printf("next: %ld\n", next_secs);
  }
  nowtm.tm_min = 0;
  nowtm.tm_sec = 0;
  time_t next_time =  mktime(&nowtm);
  next_time += next_secs;

  localtime_r(&next_time, &nowtm);
  printf("%s\n", asctime(&nowtm));
  return next_time;
}

/**********************************************************************/

bool rt_hour_changed(time_t t1, time_t t2)
{
  struct tm mytm1;
  struct tm mytm2;
  localtime_r(&t1, &mytm1);
  localtime_r(&t2, &mytm2);
  if (mytm1.tm_hour == mytm2.tm_hour)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**********************************************************************/

bool rt_day_changed(time_t t1, time_t t2)
{
  struct tm mytm1;
  struct tm mytm2;
  localtime_r(&t1, &mytm1);
  localtime_r(&t2, &mytm2);
  if (mytm1.tm_yday == mytm2.tm_yday)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**********************************************************************/

bool rt_week_changed(time_t t1, time_t t2)
{
  struct tm mytm1;
  struct tm mytm2;
  localtime_r(&t1, &mytm1);
  localtime_r(&t2, &mytm2);
  // This works no matter which order the times are given in
  if ( ((mytm1.tm_wday == 0) && (mytm1.tm_wday == 6)) ||
     ((mytm2.tm_wday == 0) && (mytm1.tm_wday == 6)) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**********************************************************************/

bool rt_month_changed(time_t t1, time_t t2)
{
  struct tm mytm1;
  struct tm mytm2;
  localtime_r(&t1, &mytm1);
  localtime_r(&t2, &mytm2);
  if (mytm1.tm_mon == mytm2.tm_mon)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**********************************************************************/

bool rt_year_changed(time_t t1, time_t t2)
{
  struct tm mytm1;
  struct tm mytm2;
  localtime_r(&t1, &mytm1);
  localtime_r(&t2, &mytm2);
  if (mytm1.tm_year == mytm2.tm_year)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**********************************************************************/

void rt_make_day_file_name(time_t the_time, char *fname, int size_fname, const char *pre, const char *post, const char *loghome)
{
  if (loghome == NULL)
  {
    loghome = "./";
  }
  if (pre == NULL)
  {
    pre = "";
  }
  if (post == NULL)
  {
    post = "";
  }
  char buf1[30];
  struct tm mytm;
  localtime_r(&the_time, &mytm);
  strftime(buf1, sizeof(buf1), "%Y%m%d", &mytm);
  snprintf(fname, size_fname, "%s/%s%s%s", loghome, pre, buf1, post);
}

/**********************************************************************/

FILE *rt_open_day_history_file(const char * pre, const char *post, const char *loghome, FILE *fp)
{
  char fname[500];
  if (fp != NULL)
  {
    fclose(fp);
  }
  time_t now = time(NULL);
  rt_make_day_file_name(now, fname, sizeof(fname), pre, post, loghome);

  //printf("Opening %s\n", fname);
  fp = fopen(fname, "a");
  if (fp == NULL)
  {
    perror(fname);
  }
  return fp;
}

/**********************************************************************


int main(int argc, char *argv[])
{
  calc_next_sample_time(time(NULL), atol(argv[1]));
} 

************************/


