
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "rtcommon.h"

int get_numbered_file(const char *dir, const char *prefix, const char *suffix, char *fname, int fname_size)
{
  if (!dir_exists(dir))
  {
    fname[0] = '\0';
    return -1;
  }
  for (int i=1; true; i++)
  {
    snprintf(fname, fname_size, "%s/%s%d%s", dir, prefix, i, suffix);
    if (!file_exists(fname))
    {
      return 0;
    }
  } 
}

