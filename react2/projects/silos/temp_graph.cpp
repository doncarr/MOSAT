
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

FILE *log_fp = NULL;
FILE *last_fp = NULL;
FILE *dfp = NULL;

/*************************************************************************/

void spd_create_image(const char *base_name, const char *gtitle, bool window)
{
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "set yrange [0:100]\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"fecha-tiempo\"\n");
  fprintf(fp, "set ylabel \"%%humedad y temp\"\n");

  fprintf(fp, "set title \"%s\"\n", gtitle);

  // The following line is basic black and white
  //fprintf(fp, "set terminal png size 800,600 xffffff x000000 x000000 x000000\n");
  // The following is default, probably black and white with red data
  //fprintf(fp, "set terminal png size 800,600\n");
  //The follow line is light blue background, blue font, magenta data line
  if (!window)
  {
    fprintf(fp, "set terminal png size 1000,720\n");
                    //xadd8e6 x0000ff xdda0dd x9500d3\n");
    //fprintf(fp, "set terminal png small color\n");
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "set xdata time\n");
  //fprintf(fp, "set timefmt \"%%Y-%%m-%%dT%%H:%%M:%%S\"\n");
  fprintf(fp, "set timefmt \"%%H:%%M:%%S\"\n");
  fprintf(fp, "plot \"%s.txt\" using 2:5 with lines lw 2 title \"TempAs\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:6 with lines lw 2 title \"HumAs\"", base_name);
  fprintf(fp, "\n");

  fclose(fp);
  char command[500];
  if (window)
  {
    snprintf(command, sizeof(command), "gnuplot -persist %s", fname);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > %s.png", fname, base_name);
  }

  printf("Cmd: %s\n", command);

  system(command);

}

/*********************************************************/

int main(int argc, char *argv[])
{

  const char *input_file = "";

  if (argc > 1)
  {
    input_file = argv[1];
  }
 else
  {
    printf("\nError\n\n");
    exit(1);
  }

  spd_create_image(input_file, "Temp Humidity", true);
  spd_create_image(input_file, "Temp Humidity", false);

  return(0);
}


