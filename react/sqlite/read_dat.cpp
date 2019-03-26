
#include <stdio.h>

#include "rtcommon.h"
#include "arg.h"

int read_dat_file(const char *fname)
{
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    return -1;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    int argc;
    char *argv[25];
    char tmp[300];
    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    for (int j=0; j < argc; j++)
    {
      printf("%s, ", argv[j]);
    }
    printf("\n");
  }
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("You must specify a file\n");
    return 0;
  }
  
  read_dat_file(argv[1]);
  return 0;
}
