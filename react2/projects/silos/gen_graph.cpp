
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "../../silodata.h"

FILE *log_fp = NULL;
FILE *last_fp = NULL;
FILE *dfp = NULL;

/*************************************************************************/

void spd_create_image(const char *base_name, const char *gtitle, bool window)
{
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"fecha-tiempo\"\n");
  fprintf(fp, "set ylabel \"%%humedad y temp\"\n");

  fprintf(fp, "set title \"%s\"\n", gtitle);

  // The following line is basic black and white
  //fprintf(fp, "set terminal png size 800,600 xffffff x000000 x000000 x000000\n");
  // The following is default, probably black and white with red data
  //fprintf(fp, "set terminal png size 800,600\n");
  //The follow line is light blue background, blue font, magenta data line
  if (!window)
  {
    fprintf(fp, "set terminal png size 1000,720\n");
                    //xadd8e6 x0000ff xdda0dd x9500d3\n");
    //fprintf(fp, "set terminal png small color\n");
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "set xdata time\n");
  //fprintf(fp, "set timefmt \"%%s\"\n");
  fprintf(fp, "set timefmt \"%%Y/%%m/%%d-%%H:%%M:%%S\"\n");
  fprintf(fp, "plot \"%s.txt\" using 1:2 with lines lw 2 title \"temperatura\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:3 with lines lw 2 title \"humedad\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:4 with lines lw 2 title \"ventilador\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:3 with lines lw 2 title \"low\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:5 with lines lw 2 title \"high\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:6 with lines lw 2 title \"vhigh\"", base_name);
  fprintf(fp, "\n");

  fclose(fp);
  char command[500];
  if (window)
  {
    snprintf(command, sizeof(command), "gnuplot -persist %s", fname);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > %s.png", fname, base_name);
  }

  printf("Cmd: %s\n", command);

  system(command);

}

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

int main(int argc, char *argv[])
{

  const char *input_file = "poncitlan_dat.txt";

  if (argc > 1)
  {
    input_file = argv[1];
  }
 else
  {
    printf("\nUsage: gen_graph input_file_name [start_date_time]\n\n");
    printf("start_date_time in YYYY/MM/DD-HH:MM:SS\n\n");
    printf("This program will create an output file for gnuplot of the form\n\n");
    printf("Each line in the input file must be of the form:\n");
    printf("tag:value,unix_time_stamp\n\n");
    printf("Two example lines:\n\n");
    printf("temp_amb:10,1261821000\n");
    printf("hum_rel:72.5,1261821000\n\n");
    printf("Each line of the output file will be of the form:\n\n");
    printf("YYYY/MM/DD-HH:MM:SS<tab>temp_value<tab>humidity_value\n\n");
    printf("temp_amb and hum_rel lines in the input are matched, and if time stamps are equal,\n"); 
    printf("     used to create an output line\n\n");
    printf("If no start_date_time is given, the whole file is output.\n");
    printf("After the file is created, gnuplot is called to create BOTH a png file and interactive screen output\n");
    exit(1);
  }

  log_fp = fopen(input_file, "r");
  if (log_fp == NULL) 
  {
    perror(input_file);
    exit(1);
  }

  FILE *out_fp = fopen("poncitlan_out.txt", "w");
  if (out_fp == NULL) 
  {
    perror("poncitlan_out.txt");
    exit(1);
  }

  FILE *dfp = fopen("poncitlan_d.txt", "r");
  if (dfp == NULL) 
  {
    perror("poncitlan_d.txt");
    exit(1);
  }

  char line[300];
  char tag[50];
  char value1[50];
  char value2[50];
  char valued[50];
  time_t the_time1;
  time_t the_time2;
  time_t start_time;
  time_t d_time;
  time_t last_time = 0;
  int line_num = 0;
  int n_errors = 0;
  int last_gap =0;
  struct tm mytm;
  bool fan_on, next_fan_on;
  float fan_val;
  

//temp_amb:26,1261249200
//hum_rel:24,1261249200

  if (argc > 2)
  {
    strptime(argv[2], "%Y/%m/%d-%H:%M:%S", &mytm);
    start_time = mktime(&mytm);
  
    for (int i=0; NULL !=  fgets(line, sizeof(line), log_fp); i++)
    {
      read_dat_line(line, tag, sizeof(tag), value1, sizeof(value1), &the_time1);
      if ((0 == strcmp(tag, "temp_amb")) && the_time1 >= start_time) 
      {
        fgets(line, sizeof(line), log_fp);
        break;
      }
     
    }
  }

  fan_on = false;
  next_fan_on = false;
  fan_val = 50.0;
  float tnow = 0, hnow = 0, tlast = 0, hlast = 0;
  int hcount = 0;
  int tcount = 0;
  while (NULL !=  fgets(line, sizeof(line), dfp))
  {
    read_dat_line(line, tag, sizeof(tag), valued, sizeof(valued), &d_time);
    printf("--- tag: %s, value:%s, %ld\n", tag, valued, d_time);
    if (0 == strcmp(tag, "ventilador")) 
    {
      next_fan_on = valued[0] != '0';
      printf("next_fan_on = %s\n", next_fan_on ? "true" : "false");
      break;
    }
  }

  for (int i=0; NULL !=  fgets(line, sizeof(line), log_fp); i++)
  {
     //int read_dat_line(const char *line, char *tag, int tag_max,
           //char *value, int value_max,
           //time_t *the_time)

    line_num++;
    printf("-------------\n");
    read_dat_line(line, tag, sizeof(tag), value1, sizeof(value1), &the_time1);
    printf("tag: %s\nvalue1: %s\ntime1: %ld\n", tag, value1, the_time1);
    if (0 != strcmp(tag, "temp_amb")) 
    {
      printf("Line %d: @@@@@@@@@@@@@@@@@@@@@@ NOT temp_amb: %s\n", line_num, tag);
      n_errors++;
      continue;
    }
    tnow = atof(value1);
    if (NULL !=  fgets(line, sizeof(line), log_fp))
    {
      line_num++;
      read_dat_line(line, tag, sizeof(tag), value2, sizeof(value2), &the_time2);
      printf("tag: %s\nvalue2: %s\ntime2: %ld\n", tag, value2, the_time2);
      if (0 != strcmp(tag, "hum_rel")) 
      {
        printf("Line %d: &&&&&&&&&&&&&&&&&&&& NOT hum_rel: %s\n", line_num, tag);
        n_errors++;
        continue;
      }
      hnow = atof(value2);
    }
    if (the_time1 != the_time2) 
    {
      printf("Line %d: ^^^^^^^^^^^^^^^^^^^ Bad times\n", line_num);
      n_errors++;
      continue;
    }
    if ((last_time != 0) && ((the_time1 - last_time) > 1000))
    {
      printf("Line %d: $$$$$$$$$$$$$$ Big gap in time\n", line_num);
      last_gap = line_num;
    }
    last_time = the_time1;

    if (the_time1 > d_time)
    {
      fan_on = next_fan_on;
      fan_val = fan_on ? 55.0 : 50.0;
      printf("fan_val now is: %0.1f, fan is %s, next_fan_on\n", fan_val, fan_on ? "on" : "off");
      while (NULL !=  fgets(line, sizeof(line), dfp))
      {
        read_dat_line(line, tag, sizeof(tag), valued, sizeof(valued), &d_time);
        printf("--- tag: %s, value:%s, %ld\n", tag, valued, d_time);
        if (0 == strcmp(tag, "ventilador")) 
        {
          printf("next_fan_on = %s\n", next_fan_on ? "true" : "false");
          next_fan_on = valued[0] != '0';
          break;
        }
      }
    }
    //struct tm *localtime_r(const time_t *timep, struct tm *result);
    char time_str[100];

    localtime_r(&the_time1, &mytm);

    strftime(time_str, sizeof(time_str), "%Y/%m/%d-%H:%M:%S", &mytm);
    printf("Time: %s\n", time_str);



    if (argc > 2)
    {
      if (the_time1 >= (start_time + (60*60*24)))
      {
        break;
      }
    } 

    hcount++;
    if (hcount > 1)
    {
      if (fabs(hnow - hlast) > 2) 
      {
        hnow = hlast;
        hcount = 0;
      }
    }

    tcount++;
    if (tcount > 1)
    {
      if (fabs(tnow - tlast) > 1) 
      {
        tnow = tlast;
        tcount = 0;
      }
    }

    fan_val = ((hnow > 70) && (hnow < 78)) ? 55.0 : 50.0;

    fprintf(out_fp, "%s\t%0.1f\t%0.1f\t%0.1f\n", time_str, tnow, hnow, fan_val);
    tlast = tnow;
    hlast = hnow;

  }
  fclose(out_fp);

  spd_create_image("poncitlan_out", "Humedad Temperatura", true);
  spd_create_image("poncitlan_out", "Humedad Temperatura", false);

  printf("**********\n");
  printf("%d errors\n", n_errors);
  printf("%d last gap\n", last_gap);
  printf("**********\n");
    
    /****
    if (0 == strncasecmp("float", type, 5))
    {
      rv = write_float_to_web(url, tag, atof(value), the_time, key);
      if (rv != 0)
      {
         perror("write_float_to_web\n");
         printf("Return Value: %d\n", rv);
      }
    }
    else if (0 == strncasecmp("bool", type, 4))
    {
      rv = write_bool_to_web(url, tag, value[0] == '1', the_time, key);
      if (rv != 0)
      {
         perror("write_float_to_web\n");
         printf("Return Value: %d\n", rv);
      }
    }
    else
    {
      printf("****** Line %d, Bad type: %s\n", i, type);
    }
    ***/
      //rv1 = write_float_to_web(url, "temp_amb", temps[sim_idx], now, key);
      //rv2 = write_float_to_web(url, "hum_rel", hums[sim_idx], now, key);
      //rv1 = write_bool_to_web(url, "ventilador", fans_on, now, key);

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



