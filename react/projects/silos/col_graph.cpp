
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

void spd_create_image(const char *base_name, const char *gtitle, bool window, int n_cols)
{
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  //fprintf(fp, "set   autoscale\n");
  fprintf(fp, "set auto x\n");
  fprintf(fp, "set yrange [25:55]\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"fecha-tiempo\"\n");
  fprintf(fp, "set ylabel \"temperatura C\"\n");

  //set datafile separator "," 
  //set style data histogram 
  //set style fill solid 

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
  fprintf(fp, "plot "); 
  for (int i=0; i < n_cols; i++)
  {
    if (i>0) fprintf(fp, ", ");
    fprintf(fp, "\"%s.txt\" using 2:%d with lines lw 2 title \"Sensor %d\"", base_name, i+3, i+1);
  } 
  fprintf(fp, "\n");
  /**
  fprintf(fp, "  \"%s.txt\" using 2:3 with lines lw 2 title \"Sensor 1\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:4 with lines lw 2 title \"Sensor 2\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:5 with lines lw 2 title \"Sensor 3\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:6 with lines lw 2 title \"Sensor 4\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:7 with lines lw 2 title \"Sensor 5\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:8 with lines lw 2 title \"Sensor 6\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:9 with lines lw 2 title \"Sensor 7\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:10 with lines lw 2 title \"Sensor 8\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:11 with lines lw 2 title \"Sensor 9\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:12 with lines lw 2 title \"Sensor 10\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:13 with lines lw 2 title \"Sensor 11\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 2:14 with lines lw 2 title \"Sensor 12\"", base_name);
  fprintf(fp, "\n");
  **/

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
  int n_cols;

  if (argc > 1)
  {
    input_file = argv[1];
  }
 else
  {
    printf("\nError, you must include the base file name\n\n");
    exit(1);
  }

  if (argc > 2)
  {
    n_cols = atoi(argv[2]);
  }
 else
  {
    n_cols = 12;
  }
  printf("\nGraphing %d columns.\n", n_cols);

  spd_create_image(input_file, "Temperature Column", true, n_cols);
  spd_create_image(input_file, "Temperature Column", false, n_cols);

  return(0);
}


