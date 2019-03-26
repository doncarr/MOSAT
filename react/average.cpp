/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#include "rtcommon.h"
#include "arg.h"

char path_to_dir[500];
char data_file[500];
char output_file[500];

/*************************************************************************/

void average_it(void)
{
  char line[300];

  FILE *fp = fopen(data_file, "r");
  if (fp == NULL)
  {
    printf("Can't open data file: %s\n", data_file);
    return;
  }

  FILE *fpout = fopen(output_file, "w");
  if (fpout == NULL)
  {
    printf("Can't open output file: %s\n", output_file);
    return;
  }

  int count = 0;
  double total = 0.0;
  double time_zero;

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];
    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '\t', 25);
    if (argc < 2)
    {
      continue;
    }
    double time = atof(argv[0]);
    double val  = atof(argv[1]);
    if (i == 0)
    {
      time_zero = time;
    }

    total += val;
    count++;
    if (count >= 10)
    {
      fprintf(fpout, "%0.1lf\t%lf\n", time - time_zero, total / 10.0);
      count = 0;
      total = 0.0;
    }
  }
  fclose(fp);
  fclose(fpout);
}

/***********************************************************************/

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Tienes que entrar el nombre de archivo de datos (ejemplo opa.txt)\n");
    exit(0);
  }

  printf("Reading path . . . \n");
  FILE *fp = fopen("lasttest.txt", "r");

  if (fp == NULL)
  {
    printf("Can't open file: last.txt\n");
    exit(0);
  }
  if (NULL == fgets(path_to_dir, sizeof(path_to_dir), fp))
  {
    printf("No first line in lasttest.txt\n");
    exit(0);
  }
  ltrim(path_to_dir);
  rtrim(path_to_dir);
  printf("Using path: %s\n", path_to_dir);
  fclose(fp);


  safe_strcpy(data_file, path_to_dir, sizeof(data_file));
  safe_strcat(data_file, "/", sizeof(data_file));
  safe_strcat(data_file, argv[1], sizeof(data_file));
  safe_strcpy(output_file, path_to_dir, sizeof(output_file));
  safe_strcat(output_file, "/", sizeof(output_file));
  safe_strcat(output_file, "avg.txt", sizeof(output_file));
  average_it();

}
