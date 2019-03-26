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

#include <stdio.h>
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
char peak_file[500];
char png_file[500];

typedef struct
{
  double lower_limit;
  double upper_limit;
  //double lower_integ;
} integ_range_t;

struct myline_t
{
  double x1;
  double y1;
  double x2;
  double y2;
  double area;
};

/*************************************************************************/

void create_image(const char *data, const char *png, myline_t *line, bool window)
{
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"time seconds\"\n");
  fprintf(fp, "set ylabel \"volts\"\n");
  if (line != NULL)
  {
    fprintf(fp, "set arrow nohead from %lf,%lf to %lf,%lf\n",
		    line->x1, line->y1, line->x2, line->y2);
    fprintf(fp, "set title \"Peak - area = %0.3lf\"\n", line->area);
    //fprintf(fp, "set arrow nohead from 2000,80 to 3000,40\n");
  }
  else
  {
    fprintf(fp, "set title \"All Data\"\n");
  }

  // The following line is basic black and white
  //fprintf(fp, "set terminal png size 800,600 xffffff x000000 x000000 x000000\n");
  // The following is default, probably black and white with red data
  //fprintf(fp, "set terminal png size 800,600\n");
  //The follow line is light blue background, blue font, magenta data line
  if (!window)
  {
    //fprintf(fp, "set terminal png size 800,600 xadd8e6 x0000ff xdda0dd x9500d3\n");
    fprintf(fp, "set terminal png\n");
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "plot \"%s\" with linespoints pt 0\n", data);
  fclose(fp);
  char command[500];
  if (window)
  {
    snprintf(command, sizeof(command), "gnuplot -persist %s", fname);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > %s", fname, png);
  }
  system(command);
}

/*************************************************************************/

void integrate_it(const char *fname, int n_ranges, integ_range_t ranges[])
{
  double total = 0.0;
  double last_val;
  double last_time;
  char line[300];
  int range_num = 0;
  char peakn[2];
  int npeaks = 0;
  peakn[0] = '0';
  peakn[1] = '\0';
  FILE *fp_peak = NULL;

  printf("Opening data file: %s\n", fname);
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    printf("Can't open data file: %s\n", fname);
    return;
  }

  printf("Opening output file: %s\n", output_file);
  FILE *fpout = fopen(output_file, "w");
  if (fpout == NULL)
  {
    printf("Can't open output file: %s\n", fname);
    return;
  }


  myline_t plines[n_ranges];
  double lower_limit = ranges[range_num].lower_limit;
  double upper_limit = ranges[range_num].upper_limit;
  //double lower_integ = ranges[range_num].lower_integ;
  bool first = true;
  float y1, y2;
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
    if (time > upper_limit)
    {
      /* Skip to next range */
      y2 = val;
      double diff = ((y1 - y2) * (upper_limit - lower_limit)) / 2.0;
      double total2 = total + diff;
      printf(
 "(%lf < x < %lf) y1 = %lf, y2 = %lf, \ntotal = %lf, dif = %lf, total2 = %lf\n",
		    lower_limit, upper_limit, y1, y2, total, diff, total2);
      fprintf(fpout,
 "(%lf < x < %lf) y1 = %lf, y2 = %lf, \ntotal = %lf, dif = %lf, total2 = %lf\n",
		    lower_limit, upper_limit, y1, y2, total, diff, total2);
      plines[range_num].x2 = time;
      plines[range_num].y2 = val;
      plines[range_num].area = total2;
      total = 0.0;
      first = true;
      range_num++;
      if (range_num >= n_ranges)
      {
        break;
      }
      lower_limit = ranges[range_num].lower_limit;
      upper_limit = ranges[range_num].upper_limit;
      //lower_integ = ranges[range_num].lower_integ;
    }
    if ((time > lower_limit) && (time <= upper_limit) && (i != 0))
    {
      if (first)
      {
        y1 = last_val;
	first = false;

        plines[range_num].x1 = last_time;
        plines[range_num].y1 = last_val;
        peakn[0]++;
	npeaks++;
        safe_strcpy(peak_file, path_to_dir, sizeof(peak_file));
        safe_strcat(peak_file, "/", sizeof(output_file));
        safe_strcat(peak_file, "peak", sizeof(peak_file));
        safe_strcat(peak_file, peakn, sizeof(peak_file));
        safe_strcat(peak_file, ".txt", sizeof(peak_file));
        printf("Opening peak file: %s\n", peak_file);
        if (fp_peak != NULL) fclose(fp_peak);
        fp_peak = fopen(peak_file, "w");
        if (fp_peak == NULL)
        {
          printf("Can't open data file: %s\n", fname);
          return;
        }
	//printf("%s:%d\n", __FILE__, __LINE__);
      }
      //printf("%s:%d\n", __FILE__, __LINE__);
      fprintf(fp_peak, "%s", line);
      //printf("%s:%d\n", __FILE__, __LINE__);
      total += (time - last_time) *
	      (((val - y1) + (last_val - y1)) / 2.0);
      //printf("%s:%d\n", __FILE__, __LINE__);
    }
    last_val = val;
    last_time = time;
  }
  if (range_num != n_ranges)
  {
    printf("Error in range file, not all ranges found\n");
    fprintf(fpout, "Error in range file, not all ranges found\n");
  }
  if (fp_peak != NULL) fclose(fp_peak);
  fclose(fpout);
  for (int i=0; i < npeaks; i++)
  {
    snprintf(peak_file, sizeof(peak_file), "%s/peak%c.txt", path_to_dir, '0' + i + 1);
    snprintf(png_file, sizeof(png_file), "%s/peak%c.png", path_to_dir, '0' + i + 1);
    printf("Creating image: %s\n", png_file);
    create_image(peak_file, png_file, &plines[i], true);
    create_image(peak_file, png_file, &plines[i], false);
  }
}

/***********************************************************************/

int read_limits(integ_range_t ranges[], int max)
{
  int n_ranges = 0;


  printf("Reading Limits . . . \n");
  FILE *fp = fopen("limits.txt", "r");

  if (fp == NULL)
  {
    printf("Can't open file: limits.txt\n");
    exit(0);
  }

  char line[300];
  char tmp[300];
  int argc;
  char *argv[25];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '\t', 25);
    if (argc < 2)
    {
      continue;
    }
    ranges[n_ranges].lower_limit = atof(argv[0]);
    ranges[n_ranges].upper_limit = atof(argv[1]);
    //ranges[n_ranges].lower_integ = atof(argv[2]);
    printf("Limits found: (%lf < x < %lf)\n",
		   ranges[n_ranges].lower_limit,
		   ranges[n_ranges].upper_limit);
		   //ranges[n_ranges].lower_integ);
    n_ranges++;
  }
  return n_ranges;
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

  system("gedit limits.txt");
  integ_range_t ranges[25];
  int n_ranges = read_limits(ranges, 25);
  printf("%d ranges found in limit file\n", n_ranges);

  if (argc < 2)
  {
    printf(
     "You must enter file name\n");
    exit(0);
  }

  char opapng[500];
  safe_strcpy(data_file, path_to_dir, sizeof(data_file));
  safe_strcat(data_file, "/", sizeof(data_file));
  safe_strcat(data_file, argv[1], sizeof(data_file));
  snprintf(opapng, sizeof(opapng), "%s/opa.png", path_to_dir);
  printf("Creating image: %s\n", opapng);
  create_image(data_file, opapng, NULL, false);
  safe_strcpy(output_file, path_to_dir, sizeof(output_file));
  safe_strcat(output_file, "/", sizeof(output_file));
  safe_strcat(output_file, "integ.txt", sizeof(output_file));
  integrate_it(data_file, n_ranges, ranges);

}
