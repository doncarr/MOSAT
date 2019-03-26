
#include <stdio.h>
#include <stdlib.h>


int connect_wireless(char *essid, char *this_ip, char *ping_ip)
{
  char cmd[100];
  int retval;
  int n = 0;
  snprintf(cmd, sizeof(cmd), "ifconfig wlan0 up");
  printf("%s\n", cmd);
  while (1)
  {
    retval = system(cmd);
    if (retval == 0)
    {
      break;
    }
    printf("Can't bring wlan0 up, will try again (%d)\n", n++);
    sleep(2);
  }

  snprintf(cmd, sizeof(cmd), "iwconfig wlan0 essid %s", essid);
  printf("%s\n", cmd);
  n = 0;
  while (1)
  {
    retval = system(cmd);
    if (retval == 0)
    {
      break;
    }
    printf("Can't set essid \"%s\", will try again (%d)\n", essid, n++);
    sleep(2);
  }
  snprintf(cmd, sizeof(cmd), "ifconfig wlan0 %s", this_ip);
  printf("%s\n", cmd);
  n = 0;
  while (1)
  {
    retval = system(cmd);
    if (retval == 0)
    {
      break;
    }
    printf("Can't configure wlan0 for %s (%d)\n", this_ip, n++);
    sleep(2);
  }
  snprintf(cmd, sizeof(cmd), "ping -c 1 %s", ping_ip, n++);
  printf("%s\n", cmd);
  n = 0;
  while (1)
  {
    retval = system(cmd);
    if (retval == 0)
    {
      break;
    }
    printf("Can't can' ping %s (%d)\n", ping_ip, n++);
    sleep(2);
  }
  printf("Connected!!!!!!!!!\n", ping_ip, n++);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc > 1)
    connect_wireless("CARR_UDG", "192.168.1.55", "192.168.1.1");
  else
    connect_wireless("CARR_DLINK_DI624", "192.168.0.55", "192.168.0.1");
}
