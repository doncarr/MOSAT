
#include <stdio.h>
#include <sqlite3.h>

#include "rtcommon.h"
#include "arg.h"
#include "gen_common.h"


/***************************/

int insert_from_dat_file(sqlite3 *sqdb, 
                 const char *fname, write_one_t write_fn)
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
    printf("%s\n", line);
    write_fn(sqdb, argc, argv);
  }
  return 0;
}


/***************************/

