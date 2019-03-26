
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/rtcommon.h"

void create_profile_dir(const char *base_dir, char *dirname, int dir_sz)
{
  char cmd[200];
  if (!dir_exists(base_dir))
  {
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", base_dir);
    system(cmd);
  }
  for (int k=1; true; k++)
  {
    snprintf(dirname, dir_sz, "%s/profile%d", base_dir, k);
    if (!dir_exists(dirname))
    {
      snprintf(cmd, sizeof(cmd), "mkdir %s", dirname);
      system(cmd);
      snprintf(cmd, sizeof(cmd), "date >%s/date.txt", dirname);
      system(cmd);
      printf("Using: %s\n", dirname);
      break;
    }
  }
}

