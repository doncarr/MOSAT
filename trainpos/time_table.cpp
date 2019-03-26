
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <rtcommon.h>
#include <arg.h>

#include "time_table.h"
#include "ap_config.h"

/********************************************************/
time_table_t::time_table_t(void)
{
  n_departures = 0;
  next_match = 0;
}
/********************************************************/

// Obviously, this needs to be fixed to read a different timetable
// if necessary.
void time_table_t::next_day(void)
{
  for (int i=0; i < n_departures; i++)
  {
    this->matched[i] = false;
    this->times[i] += 24 * 60 * 60;
  }
  next_match = 0; 
}

/********************************************************/

// We will need hooks so that each location can specify the
// matching algorithm. Here, if it is within 3 minutes of a 
// scheduled departure, we match it.
bool time_table_t::is_a_match(time_t scheduled_time, time_t actual_time)
{
   return ((actual_time > (scheduled_time - 180)) || (actual_time < (scheduled_time + 180)));  
}

/********************************************************/

const char *time_table_t::match_departure(time_t actual_departure_time)
{
  //int found = false;
  int count = 0;
  for (int i=(next_match); i < n_departures; i++)
  {
    if (is_a_match(this->times[i], actual_departure_time))
    {
       //found = true;
       this->matched[i] = true;
       if (count > 0) 
       {
         printf("******** Error, departures %d skipped\n", count);
       }
       next_match = i + 1;
       return this->train_id[i];
    }
    count++; 
  }
  // If there is an extra departure that does not mach anything, then we need
  // need to fix the code to go back to where we started.
  return "?????";
}

/********************************************************/

void time_table_t::read_day(void)
{
  time_t today = time(NULL);
  char line[300];

  struct tm mytm;
  localtime_r(&today, &mytm);

  // Obviously, this needs to be modified with a hook to determine the
  // correct timetable file. For each country, the holidays are different,
  // and in some places, they will run a Sunday schedule on holidays, so it
  // is very location dependent.
  const char *fname = ap_config.get_config("TIMETABLE_FILE");
  if (fname == NULL)
  {
     fname = "timetable.txt";
  }

  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
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

    localtime_r(&today, &mytm);

    if (NULL == strptime(argv[1], "%T", &mytm))
    {
      perror(argv[1]);
      exit(0);
    }

    printf("---%d: %s, %s---\n", n_lines, argv[0], argv[1]);
    times[n_lines] = mktime(&mytm);
    safe_strcpy(train_id[n_lines], argv[0], sizeof(train_id[n_lines])); 
    n_lines++;
    if (n_lines >= 500)
    {
      printf("Error: Maximum reached\n");
      break;
    }
  }
  n_departures = n_lines;
}

/********************************************************/

