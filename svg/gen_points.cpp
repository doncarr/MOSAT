
#include <stdio.h> 
#include <stdlib.h>

const char *units[] = {"psi", "amps", "degC", "rpm"};
const char *lo_desc[] = {"closed", "off", "ok", "ok"};
const char *hi_desc[] = {"open", "on", "alarm", "failed"};
const char *path = "./dbfiles";

FILE *open_file(FILE *fp, const char *name)
{
  char fname[200];
  snprintf(fname, sizeof(fname), "%s/%s", path, name);
  if (fp != NULL) fclose(fp);
  fp = fopen(fname, "a");
  if (fp == NULL)
  {
    perror(fname);
    exit(-1);
  }
  return fp;
}

int main(int argc, char *argv[])
{
  FILE *fp = NULL;
  system("cp ../atemajac_pumps2/dbfiles/*.dat ./dbfiles/");
  fp = open_file(fp, "ai.dat");
  for (int i=0; i < 1000; i++)
  {
    fprintf(fp, "AI_%03d|Analong Input %03d|%s|0|0|%d|0|10|0|100|3|-12.50|0|0|0|0|0|0|0|0|0|\n", i, i, units[i %4], i);
  }
  fp = open_file(fp, "ao.dat");
  for (int i=0; i < 100; i++)
  {
    fprintf(fp, "AO_%03d|Analong Output %03d|%%|0|0|%d|0|10|0|10|1|0|100|\n", i, i, i);
  }
  fp = open_file(fp, "di.dat");
  for (int i=0; i < 1000; i++)
  {
    fprintf(fp, "DI_%03d|Discrete Input %03d|0|0|%d|%s|%s|N|N|\n", i, i, i, lo_desc[i %4], hi_desc[i % 4]);
  }
  fp = open_file(fp, "do.dat");
  for (int i=0; i < 300; i++)
  {
    fprintf(fp, "DO_%03d|Discrete Onput %03d|0|0|%d|%s|%s|\n", i, i, i, lo_desc[i %4], hi_desc[i % 4]);
  }

  fclose(fp);
}
