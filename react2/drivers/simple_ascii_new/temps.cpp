
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <semaphore.h>
#include <pthread.h>

#include "rt_serial.h"
#include "utimer.h"

sem_t read_mutex_sem;

float last_temps[3] = {20.0, 20.0, 20.0};
float the_temps[3];

FILE *log_fp = NULL;

/**********************************************************************/

static bool day_changed(time_t t1, time_t t2)
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
    printf("********* Day Changed\n");
    return true;
  }
}

/**********************************************************************/

static void make_day_file_name(time_t the_time, char *fname, int size_fname, const char *pre, const char *post)
{
  const char *loghome = "./log/"; 
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

static FILE *open_day_history_file(const char * pre, const char *post, FILE *fp)
{
  char fname[500];
  if (fp != NULL)
  {
    fclose(fp);
  }
  time_t now = time(NULL);
  make_day_file_name(now, fname, sizeof(fname), pre, post);

  printf("Opening %s\n", fname);
  fp = fopen(fname, "a");
  if (fp == NULL)
  {
    printf("**** Error Opening %s\n", fname);
  }
  fprintf(fp, "File opened\n");
  fflush(fp);
  return fp;
}


/*************************************************/

void *log_thread(void *data)
{
  time_t last_log_time = time(NULL);

  utimer_t utimer;
  utimer.set_interval(4000000);
  utimer.set_start_time();
  int count = 0;

  while (true)
  {
    utimer.wait_next();
    sem_wait(&read_mutex_sem);
      printf("  -- log thread in critical section . . .\n");
    sem_post(&read_mutex_sem);
    time_t now = time(NULL);

    bool day_change = day_changed(now, last_log_time);

    if (day_change) 
    {
      log_fp = open_day_history_file("temp", ".txt", log_fp);
    }

    char buf[30];
    struct tm mytm;
    localtime_r(&now, &mytm);
    strftime(buf, sizeof(buf), "%F\t%T", &mytm);
    fprintf(log_fp, "%s\t%0.1f\t%0.1f\t%0.1f\n", 
            buf, the_temps[0],the_temps[1],the_temps[2]);
    if (count > 150)
    {
      count = 0;
      fflush(log_fp);
    }
    printf("%s\t%0.1f\t%0.1f\t%0.1f\n", 
            buf, the_temps[0],the_temps[1],the_temps[2]);
    last_log_time = now;

    count++;
  }

}

/*************************************************/

int open_serial(void)
{
   const char *dev_name = "/dev/ttyT8S0";
   rt_verbose = 1;
   printf("Opening device %s . . . \n", dev_name);
   int device = rt_open_serial(dev_name,9600,0.0);
   printf("Device opened: %d \n", device);
   return device;
}

/*************************************************/

char next_char(int fd)
{
  char a_char;
  if (1 != read(fd, &a_char, 1))
  {
    perror("read");
    printf("End of file\n");
    exit(0);
  }
  return a_char;
}

/*************************************************/

char eat_non_white_space(int fd)
{
  char a_char;

  a_char = next_char(fd);
  while (!isspace(a_char))
  {
    a_char = next_char(fd); 
  }
  return a_char;
}

/*************************************************/

char eat_white_space(int fd)
{
  char a_char;

  a_char = next_char(fd);
  while (isspace(a_char))
  {
    a_char = next_char(fd); 
  }
  return a_char;
}

/*************************************************/

void process_buf(char *buf)
{
  printf("-- '%s'\n", buf);
  char *p;
  for (p = buf; (*p != ':') && (*p != '\0'); p++);
  if (*p != ':')
  {
    printf("Bad buffer: %s\n", buf);
    return;
  }
  if (strlen(p+1) < 1)
  {
    printf("Bad buffer: %s\n", buf);
    return;
  }
  *p = '\0';
  int n = atol(buf);
  float tmp = atof(p+1) / 100.0;
  printf("sensor[%d] = %0.1f\n", n, tmp); 
  if ((n < 0) || (n > 2)) return;
  sem_wait(&read_mutex_sem);
    printf("  -- process buf in critical section . . .\n");
    the_temps[n] = tmp;
  sem_post(&read_mutex_sem);
}

/*************************************************/

void process_file(int fd)
{
  char buf[500];
  char a_char;
  while(true)
  {
    buf[0] = eat_white_space(fd);
    int i = 1;
    a_char = next_char(fd);
    while (!isspace(a_char))
    {
      buf[i] = a_char;
      i++;
      a_char = next_char(fd);
    }
    buf[i] = '\0';
    process_buf(buf);
  }
}

/*************************************************/

void create_thread(void)
{
  printf("Initializing semaphore\n");
  if (0 != sem_init(&read_mutex_sem, 0, 1))
  {
    perror("sem_init");
  }
  printf("DONE Initializing semaphore\n");

  printf("Creating thread\n");
  pthread_t thr;
  int retval;
  retval = pthread_create(&thr, NULL, log_thread, NULL);
  if (retval != 0)
  {
    perror("can't create thread");
    exit(0);
  }
  retval = pthread_detach(thr);
  if (retval != 0)
  {
    perror("can't detach thread");
    exit(0);
  }
  printf("DONE Creating thread\n");
}

/*************************************************/

int main(int argc, char *argv[])
{
  log_fp = open_day_history_file("temp", ".txt", log_fp);
  create_thread();
  //int fd = open("temps.txt", O_RDONLY);
  int fd = open_serial(); 
  if (fd < 0)
  {
    perror("open");
    exit(0);
  }
  eat_white_space(fd);
  eat_non_white_space(fd);
  process_file(fd);
}

/*************************************************/
