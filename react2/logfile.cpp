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
#include <string.h>
#include <errno.h>

#include "rtcommon.h"

#include "logfile.h"

#include "ap_config.h"

char path_to_dir[500];
char output_file[500];
char peak_file[500];
char png_file[500];

/****************************************************************/

void logfile_t::close(void)
{
  if (fp != NULL)
  {
    fflush(fp);
    fclose(fp);
    fp = NULL;
  }
  if (fp_startup != NULL)
  {
    fflush(fp_startup);
    fclose(fp_startup);
    fp_startup = NULL;
  }
  file_print_enabled = false;
  startup_print_enabled = false;
}

/****************************************************************/

void logfile_t::perror(const char *txt)
{
  char myline[300];
  myline[0] = '\0';
  const char *sx = strerror_r(errno, myline, sizeof(myline));
  //this->vprint("errno: %d, strerror return: %s\n", errno, sx);
  this->print(txt);
  this->print(": ");
  this->print(sx);
  this->print("\n");
}

/****************************************************************/

void logfile_t::print(const char *txt)
{
  if (screen_print_enabled)
  {
    printf("%s", txt);
  }
  if (file_print_enabled)
  {
    fprintf(fp, "%s", txt);
    fflush(fp);
  }
  if (startup_print_enabled)
  {
    fprintf(fp_startup, "%s", txt);
    fflush(fp_startup);
  }
}

/****************************************************************/

void logfile_t::vprint(const char *fmt, ...)
{
  char myline[300];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  va_end(arg_ptr);
  if (screen_print_enabled)
  {
    printf("%s", myline);
  }
  if (file_print_enabled)
  {
    fprintf(fp, "%s", myline);
    fflush(fp);
  }
  if (startup_print_enabled)
  {
    fprintf(fp_startup, "%s", myline);
    fflush(fp_startup);
  }
}

/****************************************************************/

void logfile_t::previous_day(void)
{
  /* Ok, you could start a run one day, and some of the tests would
   * run after midnight. This decrements back a day and finds the
   * last test run that day.
   */

  time_t now;
  now = time(NULL);
  now -= 60*60*12;
  struct tm mytm;
  localtime_r(&now, &mytm);
  char year[10];
  char month[10];
  char day[10];
  //printf("%s %d\n", __FILE__, __LINE__);
  snprintf(year, sizeof(year), "%d", mytm.tm_year + 1900);
  snprintf(month, sizeof(month), "%02d", mytm.tm_mon + 1);
  snprintf(day, sizeof(day), "%02d", mytm.tm_mday);
  //safe_strcpy(path, loghome, sizeof(path));
  //safe_strcat(path, "/", sizeof(path));
  snprintf(path_to_day, sizeof(path_to_day),
         "%s/%s/%s/%s/", loghome, year, month, day);
}

/****************************************************************/

void logfile_t::write_final_logs(int n)
{
  char path[500];

  for(int i=0; (i < n) && (i < n_tests); i++)
  {
    //int test_num = last_test_number - i;
    //int file_num = 12 - i;
    //snprintf(path, sizeof(path), "%sexp%02d/", path_to_day, test_num);
    safe_strcpy(path, tests[i], sizeof(path));
    if (dir_exists(path))
    {
      snprintf(path, sizeof(path), "cp final%d.txt %sfinal.txt",
		      i+1, tests[i]);
      printf("%s\n", path);
      system(path);
    }
    else
    {
      printf("No dir: %s\n", path);
    }
  }
}

/****************************************************************/

void logfile_t::copy_file_to_log_dir(const char *fname, const char *name)
{
  if (!file_print_enabled)
  {
    return;
  }
  char cmd[500];
  safe_strcpy(cmd, "cp ", sizeof(cmd));
  safe_strcat(cmd, fname, sizeof(cmd));
  safe_strcat(cmd, " ", sizeof(cmd));
  safe_strcat(cmd, path_to_dir, sizeof(cmd));
  safe_strcat(cmd, "/", sizeof(cmd));
  safe_strcat(cmd, name, sizeof(cmd));
  system(cmd);
}

/****************************************************************/

FILE *logfile_t::open_file_in_log_dir(const char *name)
{
  char path[500];
  safe_strcpy(path, path_to_dir, sizeof(path));
  safe_strcat(path, "/", sizeof(path));
  safe_strcat(path, name, sizeof(path));
  return fopen(path, "w");
}

/****************************************************************/

void logfile_t::insert_in_index(const char *file_name, const char *secuencia)
{
  char datestr[40];
  char indexname[200];
  time_t now;
  now = time(NULL);
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(datestr, sizeof(datestr), "%Y-%m-%dT%H:%M:%S%z", &mytm);
  snprintf(indexname, sizeof(indexname), "%s/index.txt", loghome);
  FILE *index_fp = fopen(indexname, "a+");
  if (index_fp == NULL)
  {
    printf("Can't open index file: %s\n\r", indexname);
    return;
  }
  fprintf(index_fp, "%s\t\"%s\"\t\"%s\"\t", datestr, exp_num, secuencia); // Changed to use the ISO standard for timestamp.
  if (file_name == NULL)
  {
    fprintf(index_fp, "\n");
    fclose(index_fp);
    return;
  }
  FILE *info_fp = fopen(file_name, "r");
  if (info_fp == NULL)
  {
    printf("Can't open index file: %s\n\r", indexname);
    fprintf(index_fp, "\n");
    fclose(index_fp);
    return;
  }
  char line[300];
  for (int i=0; NULL != fgets(line, sizeof(line), info_fp); i++)
  {
    ltrim(line);
    rtrim(line);
    fprintf(index_fp, "\"%s\"\t", line);
  }
  fprintf(index_fp, "\n");
  fclose(index_fp);
  fclose(info_fp);
}

/****************************************************************/

void logfile_t::next_test(void)
{
  this->close_test();
  this->do_init(true, true);
}

/****************************************************************/

void logfile_t::close_test(void)
{
  this->create_graph();
  file_print_enabled = false;
  if (fp != NULL)
  {
    fclose(fp);
  }
  fp = NULL;
  path_to_dir[0] = '\0';
  output_file[0] = '\0';
  peak_file[0] = '\0';
  png_file[0] = '\0';
}

/****************************************************************/

logfile_t::logfile_t(void)
{
  n_tests = 0;
  fp = NULL;
  fp_startup = NULL;
  file_print_enabled = false;
  startup_print_enabled = false;

  path_to_dir[0] = '\0';
  output_file[0] = '\0';
  peak_file[0] = '\0';
  png_file[0] = '\0';
  
  this->do_init(false, true);
}

/****************************************************************/

logfile_t::logfile_t(bool print_to_file, bool print_to_screen)
{
  n_tests = 0;
  this->do_init(print_to_file, print_to_screen);
}

/****************************************************************/

void logfile_t::do_init(bool print_to_file, bool print_to_screen)
{
  file_print_enabled = print_to_file;
  screen_print_enabled = print_to_screen;
  if (file_print_enabled)
  {
    this->create_dir();
  }
  else
  {
    fp = NULL;
  }
}

/****************************************************************/

void logfile_t::create_dir(void)
{
  loghome = ap_config.get_config("loghome");
  if (loghome == NULL)
  {
    loghome = START;
    printf("Log home not specified, using: %s\n", loghome);
  }
  time_t now;
  now = time(NULL);
  struct tm mytm;
  localtime_r(&now, &mytm);
  char path[500];
  char npath[500];
  char year[10];
  char month[10];
  char day[10];
  //printf("%s %d\n", __FILE__, __LINE__);
  snprintf(year, sizeof(year), "%d", mytm.tm_year + 1900);
  snprintf(month, sizeof(month), "%02d", mytm.tm_mon + 1);
  snprintf(day, sizeof(day), "%02d", mytm.tm_mday);
  safe_strcpy(path, loghome, sizeof(path));
  safe_strcat(path, "/", sizeof(path));
  safe_strcat(path, year, sizeof(path));
  //printf("%s\n", path);
  if (!dir_exists(path))
  {
    mkdir(path, DIR_MODES);
  }
  safe_strcat(path, "/", sizeof(path));
  safe_strcat(path, month, sizeof(path));
  //printf("%s\n", path);
  if (!dir_exists(path))
  {
    mkdir(path, DIR_MODES);
  }

  safe_strcat(path, "/", sizeof(path));
  safe_strcat(path, day, sizeof(path));
  //printf("%s\n", path);

  if (!dir_exists(path))
  {
    mkdir(path, DIR_MODES);
  }

  safe_strcpy(npath, path, sizeof(npath));
  safe_strcat(npath, "/", sizeof(npath));
  safe_strcpy(path_to_day, npath, sizeof(path_to_day));
  safe_strcpy(exp_num, "exp01", sizeof(exp_num));
  safe_strcat(npath, exp_num, sizeof(npath));
  //printf("%s\n", npath);
  int n=1;
  while (dir_exists(npath) && n < 100)
  {
    n++;
    snprintf(exp_num, sizeof(exp_num), "exp%02d", n);
    //printf("%s\n", tmp);
    safe_strcpy(npath, path, sizeof(npath));
    //printf("%s\n", npath);
    safe_strcat(npath, "/", sizeof(npath));
    //printf("%s\n", npath);
    safe_strcat(npath, exp_num, sizeof(npath));
    //printf("%s\n", npath);
  }
  last_test_number = n;
  mkdir(npath, DIR_MODES);
  safe_strcat(npath, "/", sizeof(npath));
  safe_strcpy(path_to_dir, npath, sizeof(path_to_dir));
  tests[n_tests] = strdup(path_to_dir);
  MALLOC_CHECK(tests[n_tests]);
  printf("test %d: %s\n", n_tests + 1, tests[n_tests]);
  n_tests++;

  safe_strcat(npath, "log.txt", sizeof(npath));
  //printf("About to open: %s\n", npath);

  //printf("%s %d\n", __FILE__, __LINE__);
  fp = fopen(npath, "w");
  if (fp == NULL)
  {
    printf("Can't open: %s\n\r", npath);
  }

  FILE *last_fp = fopen("lasttest.txt", "w");
  if (last_fp == NULL)
  {
    printf("Can't open: %s\n\r", "lasttest.txt");
  }
  else
  {
    fprintf(last_fp, "%s\n", path_to_dir);
  }
  fclose(last_fp);
}

/*******************************************************************/

void logfile_t::set_startup_log_on(void)
{
  char path[500];

  startup_print_enabled = false;
  printf("Creating startup logfile . . . \n");
  const char *logdir = ap_config.get_config("loghome");
  if (logdir == NULL)
  {
    printf("****** Log home not specified, can not log startup\n");
    startup_print_enabled = false;
    fp_startup = NULL;
    return;
  }
  snprintf(path, sizeof(path), "%s/startuplog.txt", logdir);
  fp_startup = fopen(path, "w");
  if (fp_startup == NULL)
  {
    perror(path);
    startup_print_enabled = false;
    printf("****** Startup log not enabled, can not open file\n");
    return;
  }
  startup_print_enabled = true;
}

/*******************************************************************/

void logfile_t::set_startup_log_off(void)
{
  if (!startup_print_enabled)
  {
    return;
  }
  startup_print_enabled = false;
  if (fp_startup != NULL)
  {
    fclose(fp_startup);
    fp_startup = NULL;
  }
}

/*******************************************************************/

/*******************************************************************

int main(int argc, char *argv[])
{
  printf("%s %d\n", __FILE__, __LINE__);
  logfile_t *lg = new logfile_t("");
  lg->vprint("It worked, %s, %d\n", "Yes", 99);
  lg->print("testing\n");
  FILE *fp = lg->open_file_in_log_dir("dataout.txt");
  fprintf(fp, "This is the data file!!!\n");
  fclose(fp);
  lg->close();
}

*************************************************************************/


/*************************************************************************/

static void my_create_image(const char *data, const char *png, bool window)
{
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"time seconds\"\n");
  fprintf(fp, "set ylabel \"volts\"\n");
  fprintf(fp, "set title \"All Data\"\n");

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
    //system(command);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > %s", fname, png);
    system(command);
  }
}

/*************************************************************************/

void logfile_t::create_graph(void) //char *data_file, char *img_prefix)
{
  if (path_to_dir[0] == '\0') return;
  char data_file[500];
  snprintf(data_file, sizeof(data_file), "%s/opa.txt", path_to_dir);
  ltrim(data_file);
  rtrim(data_file);

  if (!file_exists(data_file))
  {
    return;
  }

  char opapng[500];
  snprintf(opapng, sizeof(opapng), "%s/opa.png", path_to_dir);
  printf("Image data: %s\n", data_file);
  printf("Creating image: %s\n", opapng);
  my_create_image(data_file, opapng, false);
  my_create_image(data_file, opapng, true);
}

/*************************************************************************/
