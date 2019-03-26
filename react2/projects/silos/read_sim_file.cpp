
#include <stdio.h>
#include <stdlib.h>

#include <time.h>


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


/*********************************************************/

int main(int argc, char *argv[])
{
  const char *fname = "poncitlan_out2.txt";
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    return -1;
  }

  char line[500];
  char time_str[50];
  char temp_str[20];
  char hum_str[20];
  struct tm my_tm;
  time_t my_time, last_time = 0;
  long diff;
  bool first = true;

  while (NULL !=  fgets(line, sizeof(line), fp))
  {
    printf("%s", line);
    const char *p = line;

    copy_to_delim(time_str, sizeof(time_str), p, '\t');

    p = next_field(p, '\t');
    if (p == NULL) return -1;

    copy_to_delim(temp_str, sizeof(temp_str), p, '\t');

    p = next_field(p, '\t');
    if (p == NULL) return -1;

    copy_to_delim(hum_str, sizeof(hum_str), p, '\t');

    printf("%s ** %s ** %s\n", time_str, temp_str, hum_str);

    strptime(time_str, "%Y/%m/%d-%H:%M:%S", &my_tm);

    strftime(line, sizeof(line), "%H:%M:%S", &my_tm);

    my_time = mktime(&my_tm);
    
    diff = my_time - last_time; 

    printf("%s, diff: %ld\n", line, diff);

    if (!first && (diff != 600))
    {
      printf("Error *************\n");
    }

    last_time = my_time;
    first = false;

  }
}

/*********************************************************/
