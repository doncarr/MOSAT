
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include <fcntl.h> 
#include <sys/stat.h>
#include <mqueue.h>

#include "../../silodata.h"

#define interval_mins (1) // 15 minutes.


float temps[100];
float hums[100];
mqd_t mq_fd;

/*************************************************************************/

int write_float_to_mq(const char *tag, float value, time_t the_time, const char *key)
{
  silodata_t sdata;
  snprintf(sdata.td.tag, sizeof(sdata.td.tag), "%s", tag);
  snprintf(sdata.td.type, sizeof(sdata.td.type), "float");
  snprintf(sdata.key, sizeof(sdata.key), "%s", key);
  snprintf(sdata.td.value, sizeof(sdata.td.value), "%0.1f", value);
  sdata.the_time = the_time;

  int rval = mq_send(mq_fd, (char *) &sdata, sizeof(sdata), 0);
  if (rval == -1)
  {
    perror("mq_send");
  }
  printf("Send Float Success\n");
  return 0;

}

/*************************************************************************/

int write_bool_to_mq(const char *tag, bool value, time_t the_time, const char *key)
{
  silodata_t sdata;
  snprintf(sdata.td.tag, sizeof(sdata.td.tag), "%s", tag);
  snprintf(sdata.td.type, sizeof(sdata.td.type), "bool");
  snprintf(sdata.key, sizeof(sdata.key), "%s", key);
  snprintf(sdata.td.value, sizeof(sdata.td.value), "%c", value ? '1' : '0');
  sdata.the_time = the_time;

  int rval = mq_send(mq_fd, (char *) &sdata, sizeof(sdata), 0);
  if (rval == -1)
  {
    perror("mq_send");
  }
  printf("Send Bool Success\n");
  return 0;
}

/*************************************************************************/

int main(int argc, char *argv[])
{
  time_t now;
  struct tm nowtm;
  struct tm nexttm;

  mq_fd = mq_open("/adaptivertc.react.weblog1", O_RDWR | O_CREAT, 0755, NULL);
  if (mq_fd == ((mqd_t) -1))
  {
    perror("mq_open");
  }


  FILE *fp1= fopen("out_temp_hum_sim.txt", "r");

  for (int i=0; i < 96; i++)
  {
    int n;
    char line[300];
    fgets(line, sizeof(line), fp1);
    sscanf(line, "%d\t%f\t%f", &n, &temps[i], &hums[i]);
    printf("------- %d\t%0.1f\t%0.1f\n", n, temps[i], hums[i]);
  }

  now = time(NULL);

  int next_secs = 0;
  
  localtime_r(&now, &nowtm); 
  printf("%s%ju secs since the Epoch\n", asctime(&nowtm), (uintmax_t)now); 
  int secs_after_hour = nowtm.tm_sec + (nowtm.tm_min * 60);
  printf("Secs after hour: %d\n", secs_after_hour);

  for (int i=0; ((i * interval_mins * 60) < (secs_after_hour)); i++)
  {
    printf("%d: %d\n", i, i * interval_mins * 60);   
    next_secs = (i + 1) * interval_mins * 60;   
    printf("next: %d\n", next_secs);   
  }  
  nowtm.tm_min = 0;
  nowtm.tm_sec = 0;
  time_t next_time =  mktime(&nowtm);
  next_time += next_secs;
  localtime_r(&next_time, &nowtm); 
  
  int sim_idx = (nowtm.tm_hour * 4) + (nowtm.tm_min / 15);

  sim_idx = sim_idx % 96; // make sure it is less than 96!
  
  printf("next: %s, idx = %d, next_t = %0.1f, next_h = %0.1f\n", 
       asctime(&nowtm), sim_idx, temps[sim_idx], hums[sim_idx]); 


  bool fans_on = ((hums[sim_idx] > 72) && (hums[sim_idx] < 78));
  printf("To start, fans are %s\n", fans_on ? "ON" : "OFF");

  const char *key = "examplesilokey";

  while (true)
  {
    now = time(NULL);
    if (now >= next_time)
    {
      localtime_r(&now, &nowtm); 
      printf("logged at %s, idx = %d, temp = %0.1f, hum = %0.1f\n", 
             asctime(&nowtm), sim_idx, temps[sim_idx], hums[sim_idx]); 

      write_float_to_mq("temp_amb", temps[sim_idx], now, key);
      write_float_to_mq("hum_rel", hums[sim_idx], now, key);

      bool fans_test = ((hums[sim_idx] > 72.0) && (hums[sim_idx] < 78.0));
      printf("Fans are now %s\n", fans_test ? "ON" : "OFF");
      if (fans_test != fans_on) // The state of the fans changed
      {
        printf("------------ Fans just turned %s!! ---------------\n", fans_test ?"ON":"OFF");
        fans_on = fans_test; 

        write_bool_to_mq("ventilador", fans_on, now, key);

        printf("-------------------------------------------\n");
        
      }

      next_time += interval_mins * 60;
      localtime_r(&next_time, &nexttm); 
      printf("next: %s\n", asctime(&nexttm)); 
      sim_idx = (sim_idx + 1) % 96;
    }
    sleep(1);
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



