
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>

const char *wget_str = "wget --quiet --output-document=- --post-data='column=5&total=5&time=%d&api_key=examplesilokey&temperatures=22.4,23.4,24.2,34.2,33.5' http://adaptivertc.pablasso.com/api/temperature_add";

const char *curl_str = "curl --silent --show-error --data \"column=5&total=5&time=%d&api_key=examplesilokey&temperatures=22.4,23.4,24.2,34.2,33.5\" http://adaptivertc.pablasso.com/api/temperature_add";

const char *param_str = "curl --silent --show-error --data \"api_key=examplesilokey\" http://adaptivertc.pablasso.com/api/config_get";

const char *wget_param_str = "wget --quiet --output-document=- --post-data='api_key=examplesilokey' http://adaptivertc.pablasso.com/api/config_get";

const char *param_put_str = "curl --silent --show-error --data \"api_key=examplesilokey&fan_auto=1&humidity_min=70&umidity_max=80&fan_service_min=0&fan_service_max=0\" http://adaptivertc.pablasso.com/api/config_put";

const char *lwp_str = "lwp-request \"column=5&total=5&time=%d&api_key=examplesilokey&temperatures=22.4,23.4,24.2,34.2,33.5\" http://adaptivertc.pablasso.com/api/temperature_add";


int main(int argc, char *argv[])
{
  int fds_std[2];
  int fds_err[2];
  pipe2(fds_std, O_NONBLOCK);
  pipe2(fds_err, O_NONBLOCK);
  /***
  pipe(fds_std);
  pipe(fds_err);
  fcntl(fds_std[0], F_SETFL, O_NONBLOCK);
  fcntl(fds_err[0], F_SETFL, O_NONBLOCK);
  ****/
  FILE *fp_std = fdopen(fds_std[0], "r");
  FILE *fp_err = fdopen(fds_err[0], "r");
  char cmd[800];
  char cstring[500] = {'\0'};

  if (argc > 1)
  {
    if (0 == strcmp(argv[1], "-wget"))
    {
      snprintf(cstring, sizeof(cstring), wget_str, time(NULL));
    }
    else if (0 == strcmp(argv[1], "-curl"))
    {
      snprintf(cstring, sizeof(cstring), curl_str, time(NULL));
    }
    else if (0 == strcmp(argv[1], "-lwp"))
    {
      snprintf(cstring, sizeof(cstring), lwp_str, time(NULL));
    }
    else if (0 == strcmp(argv[1], "-param"))
    {
      snprintf(cstring, sizeof(cstring), param_str, time(NULL));
    }
    else if (0 == strcmp(argv[1], "-wparam"))
    {
      snprintf(cstring, sizeof(cstring), wget_param_str, time(NULL));
    }
    else if (0 == strcmp(argv[1], "-param_put"))
    {
      snprintf(cstring, sizeof(cstring), param_put_str, time(NULL));
    }
    else
    {
      snprintf(cstring, sizeof(cstring), "%s", argv[1]);
    }
  }
  else
  {
    snprintf(cstring, sizeof(cstring), "curl http://doncarr.glo.org.mx/test_tag.txt -s -S");
  }
  //snprintf(cmd, sizeof(cmd), "ls -alhv test.cpp bla  shit >&%d 2>&1\n", fds_std[1]);
  //snprintf(cmd, sizeof(cmd), "curl http://doncarr.glo.org.mx/test_tag.txt -s -S >&%d 2>&1\n", fds_std[1]);
  snprintf(cmd, sizeof(cmd), "%s >&%d 2>&%d\n", cstring, fds_std[1], fds_err[1]);
  
  printf("cmd = %s\n", cmd);
  int retval = system(cmd);
  printf("Waiting for child to finish . . \n");
  //wait(NULL);
  printf("Child finished!!!!!!!!!!!!, retval = %d\n", retval);
  char line[300];
  int n = 0;
  while (NULL !=  fgets(line, sizeof(line), fp_std))
  {
    n++;
    printf("line %d -------------\n", n);
    printf("%s", line);
    if ((n == 1) && (0 == strcmp(line, "success"))) 
    {
      printf(" *** success returned!!\n");
    }
    char *p, *val, *tag;
    //bool found = false;
    tag = line;
    bool colon_next = true; // we first expect a colon before the first coma
    //bool found_dot = false;
    for (p = line; (*p != '\0'); p++)
    {
      if (*p == ':')
      {
        if (colon_next)
        {
          *p = '\0';
          //found = true;
          val = p + 1;
          printf("\ntag: '%s', val: %lf\n", tag, atof(val));
          colon_next = false;
        }
        else
        {
          printf("*******bad line, expected ':'\n");
          break;
        } 
      }
 
      if (*p == ',') 
      {
        if (!colon_next)
        {
          tag = p+1;
          colon_next = true;
        }
        else
        {
          printf("*******bad line, expected ','\n");
          break;
        }
      }
    }
  }
  int n_errors = 0;
  while (NULL !=  fgets(line, sizeof(line), fp_err))
  {
    printf("Oh Crap, there was an ERROR: '%s'\n", line);
    n_errors++;
  }
  printf("There were %d errors\n", n_errors);
  if (n_errors == 0)
  {
    printf("Great!!, NO ERRORS\n");
  }
}

