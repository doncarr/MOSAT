
#include <stdio.h>
#include <stdlib.h>

#include "arg.h"

//2008-12-10	15:46:48	19.2	19.0	19.0

int main(int xargc, char *xargv[])
{
  int argc, line_num;
  char **argv;

  delim_file_t df(600, 50, '\t', '#');

  if (xargc < 2) {printf("You must enter a file\n"); return 0;}

  printf("Opening %s\n", xargv[1]);
  argv = df.first(xargv[1], &argc, &line_num);

  char fname[200];
  snprintf(fname, sizeof(fname), "%s.fix", xargv[1]);
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return 0;
  }

  float last[3] = {20.0, 20.0, 20.0};
  float current[3];

  while (argv != NULL) 
  {
    printf("Valido|\n");
    if (argc >= 5)
    {
      printf("%d|%s|%s|%s|%s|%s|\n", 
         argc, argv[0], argv[1], argv[2],  argv[3],  argv[4]);
      for (int i=0; i < 3; i++)
      {
        float val = atof(argv[i+2]);
        if ((val < 0.0) || (val > 60.0))
        {
          val = last[i];
        }
        current[i] = val;
        last[i] = current[i];
      }
      printf("Converted|\n");
      fprintf(fp, "%s\t%s\t%0.1f\t%0.1f\t%0.1f\n",
        argv[0], argv[1], current[0], current[1], current[2]);
    }
    argv = df.next(&argc, &line_num);
  }
  return 0;
}
