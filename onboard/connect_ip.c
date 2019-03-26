
#include <stdio.h>
#include <stdlib.h>


int connect_ip(char *ping_ip)
{
  char cmd[100];
  int retval;
  int n = 0;

  snprintf(cmd, sizeof(cmd), "ping -c 1 %s", ping_ip, n++);
  printf("%s\n", cmd);
  n = 0;
  int net_up = 0;
  while (1)
  {
    retval = system(cmd);
    if (net_up)
    {
      if (retval == 0)
      {
        printf("Net up:  %s\n", ping_ip);
      }
      else
      {
        printf("------------- Holly cow, we lost the network!!!!!\n"); 
        system("date");
        net_up = 0;
      }
    }
    else
    {
      if (retval == 0)
      {
        printf("************** Network is back up!!!!!\n");
        system("date");
        net_up = 1;
      }
      else
      {
        printf("Net still down:  %s\n", ping_ip);
      }
    }
    sleep(2);
  }
  printf("Connected!!!!!!!!!\n", ping_ip, n++);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
     printf("You must specify the ip\n");
     exit(0);
  }
  connect_ip(argv[1]);
}
