
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


FILE *log_fp = NULL;


/*********************************************************/

void copy_to_delim(char *str_to, int str_to_size, const char *str_from, char delim)
{
  for (int i=0; i < str_to_size; i++)
  {
    if ((str_from[i] == delim) || (str_from[i] == '\n') || (str_from[i] == '\r') ||  (str_from[i] == '\0'))
    {
      str_to[i] = '\0';
      //printf("\n");
      return;
    }
    //printf("'%c' ", str_from[i]);
    str_to[i] = str_from[i];
  }
  //printf("\n");
  str_to[str_to_size -1] = '\0';
}

/*************************************************************************/

const char *next_field(const char *str, char delim)
{
  const char *p = str;
  while (*p != '\0')
  {
    if (*p == delim)
    {
      return (p+1);
    }
    p++;
  }
  return NULL;
}

/*************************************************************************/

int read_dat_line(const char *line, char *tag, int tag_max,
      char *value, int value_max,
      time_t *the_time)
{
  const char *p = line;

  copy_to_delim(tag, tag_max, p, ':');

  p = next_field(p, ':');
  if (p == NULL) return -1;

  copy_to_delim(value, value_max, p, ',');

  p = next_field(p, ',');
  if (p == NULL) return -1;

  *the_time = atol(p);

  return 0;
}

/*************************************************************************/
/*************************************************************************/


int main(int argc, char *argv[])
{

  const char *input_file = "poncitlan_dat.txt";

  if (argc > 1)
  {
    input_file = argv[1];
  }
  else
  {
    printf("\nUsage: dump_dat input_file_name [> output_file_name]\n\n");
    printf("This program will convert the time stamps to human readable format, tab delimited\n\n");
    printf("Each line in input file must be of the form:\n");
    printf("tag:value,unix_time_stamp\n\n");
    printf("Two example lines:\n\n");
    printf("temp_amb:10,1261821000\n");
    printf("hum_rel:72.5,1261821000\n\n");

    exit(1);
  }

  log_fp = fopen(input_file, "r");
  if (log_fp == NULL) 
  {
    perror(input_file);
    exit(1);
  }


  char line[300];
  char tag[50];
  char value1[50];
  time_t the_time1, last_time;
  struct tm mytm;
  bool first_time = true;

  for (int i=0; NULL !=  fgets(line, sizeof(line), log_fp); i++)
  {
     //int read_dat_line(const char *line, char *tag, int tag_max,
           //char *value, int value_max,
           //time_t *the_time)

    read_dat_line(line, tag, sizeof(tag), value1, sizeof(value1), &the_time1);
    char time_str[100];

    localtime_r(&the_time1, &mytm);
    strftime(time_str, sizeof(time_str), "%Y/%m/%d-%H:%M:%S", &mytm);
    printf("%s\t%s\t%s", time_str, value1, tag);
    if (0 == strcmp(tag, "ventilador"))
    {
      if (!first_time)
      {
        long dif = the_time1 - last_time;
        long mins = dif / 60;
        printf("\t%02ld:%02ld:%02ld", mins / 60, mins % 60, dif % 60);
      }
      first_time = false;
      last_time = the_time1;
    }
    printf("\n");
  }
  return(0);
}



//       struct tm *localtime_r(const time_t *timep, struct tm *result);
//       time_t mktime(struct tm *tm);
//
 //          struct tm {
 //              int tm_sec;         /* seconds */
 //              int tm_min;         /* minutes */
 //              int tm_hour;        /* hours */
 //              int tm_mday;        /* day of the month */
 //              int tm_mon;         /* month */
 //              int tm_year;        /* year */
 //              int tm_wday;        /* day of the week */
 //              int tm_yday;        /* day in the year */
 //              int tm_isdst;       /* daylight saving time */
 //          };



