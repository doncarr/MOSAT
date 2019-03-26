/************************************************************************
This is software to monitor and/or control trains.
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
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "trainpos.h"
#include "ap_config.h"
#include "section_reader.h"

#include "gen_displays.h"

/*
void display_alg_t::gen_alarms(time_t now)
{


}
*/


/*******************************************************************/

// This is special for India, we need to make it an installable plugin!!
void switch_direction(train_data_t *train, train_calcs_t *calcs)
{
  if (0 == strncasecmp(train->train_id, "BVL", 3))
  {
    const char * p = train->train_id;
    for (; (*p != '\0') && (!isdigit(*p)); p++);
    if (*p == '\0') return;
    int num = atol(p);
    snprintf(train->train_id, sizeof(train->train_id), "VLB%d\n", num + 1);
  }
  calcs->switched_direction = true;
}

/*******************************************************************/

display_dist_t::display_dist_t(int a_x1, int a_x2,
                               int a_y1, int a_y2, int n_sects)
{
  x1 = a_x1;
  x2 = a_x2;
  y1 = a_y1;
  y2 = a_y2;
  n_sections = n_sects;
  return_start = n_sects / 2;
  n_squares = 5 * return_start + 1;
  total_x_pixels = x2 - x1 + 1;
  //printf("x1 = %d, x2 = %d, y1 = %d, y2 = %d\n", x1, x2, y1, y2 );
  //printf("return_start = %d, n_squares = %d, total_x_pixels = %d\n", return_start, n_squares, total_x_pixels);
}

/*************************************************************************************************/


void display_dist_t::calc_xy(int section, double fraction, int *x, int *y, bool *coming_back)
{
  double extra;
  double square;

  /***
  if (fraction > 0.75)
  {
   extra = 4;
  }
  else if (fraction > 0.50)
  {
    extra = 3;
  }
  else if (fraction > 0.25)
  {
    extra = 2;
  }
  else if (fraction > 0.01)
  {
    extra = 1;
  }
  else
  {
    extra = 0;
  }
  ***/
  extra = fraction * 5.0; 

  if (section >= return_start)
  {
    *y = y1;
    *coming_back = true;
    square = (5.0 * (double(section) - double(return_start))) + extra;
    square = double(n_squares) - square - 1.0;
  }
  else
  {
    square = (5.0 * section) + extra;
    *y = y2;
    *coming_back = false;
  }
  *x = x1 + int(double(total_x_pixels) * (double(square) / double(n_squares-1)));
  //printf("s = %d, fr = %0.2lf, ex = %d, sq = %d, x = %d, y = %d\n",
  //  section, fraction, extra, square, *x, *y);
}

/*************************************************************************************************/


/**
void old_display_dist_t::calc_xy(int section, double fraction, int *x, int *y, bool *coming_back)
{
  int extra;
  int square;

  if (fraction > 0.75)
  {
   extra = 4;
  }
  else if (fraction > 0.50)
  {
    extra = 3;
  }
  else if (fraction > 0.25)
  {
    extra = 2;
  }
  else if (fraction > 0.01)
  {
    extra = 1;
  }
  else
  {
    extra = 0;
  }

  if (section >= return_start)
  {
    *y = y1;
    *coming_back = true;
    square = (5 * (section - return_start)) + extra;
    square = n_squares - square - 1;
  }
  else
  {
    square = (5 * section) + extra;
    *y = y2;
    *coming_back = false;
  }
  double pixels_per_section 
  *x = x1 + int(double(total_x_pixels) * (double(square) / double(n_squares-1)));
  //printf("s = %d, fr = %0.2lf, ex = %d, sq = %d, x = %d, y = %d\n",
  //  section, fraction, extra, square, *x, *y);
}
***/

/*******************************************************/

void gen_displays_t::init(void)
{
  const char *dfile = ap_config.get_config("DISPLAY_FILE");
  if (dfile == NULL)
  {
    dfile = "display_info.txt";
  }
  reader.read_file(dfile);
  n_displays = reader.get_n_displays();
  ddata = new display_data_t[n_displays];
  for (int i=0; i < n_displays; i++)
  {
    const display_info_t *dd = reader.get_display_data(i);
    ddata[i].set( dd->background, dd->square, dd->square_unexpected, dd->html_out,
      dd->x1, dd->x2, dd->y1, dd->y2, dd->text_offset_y1, dd->text_offset_y2, dd->n_sections);
      printf("Display[%d]: %s, %s, %s, %s, %d, %d, %d, %d, %d\n", 
         i, dd->background, dd->square, dd->square_unexpected, dd->html_out,
         dd->x1, dd->x2, dd->y1, dd->y2, dd->n_sections);
  }
  
  table_fp = fopen("line1_table.html", "w+");
  if (table_fp == NULL)
  {
    perror("line1_table.html");
    exit(0);
  }
  table_fd = fileno(table_fp);
  fseek(table_fp, 0, SEEK_SET);

  perf_fp = fopen("line1_perf.html", "w+");
  if (perf_fp == NULL)
  {
    perror("line1_perf.html");
    exit(0);
  }
  perf_fd = fileno(perf_fp);
  fseek(perf_fp, 0, SEEK_SET);

  n_sections = sections.get_n_sections();
}

/*********************************************************/

static int n_pass[] = {
57, 88, 105, 165, 198, 234, 278, 299, 333,
345, 321, 302, 281, 234, 185, 163, 111, 63,
69, 113, 158, 185, 204, 251, 282, 301, 338,
331, 304, 288, 251, 201, 185, 172, 144, 78, 
39, 81, 101, 165, 191, 211, 232, 291, 338,
332, 273, 248, 221, 181, 165, 122, 88, 55 
};

void gen_displays_t::gen_table(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains)
{

  fprintf(table_fp, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
  fprintf(table_fp, "<html>\n");
  fprintf(table_fp, "<head>\n");
  fprintf(table_fp, "<META HTTP-EQUIV=\"refresh\" CONTENT=\"1\">\n");
  fprintf(table_fp, "<meta http-equiv=\"pragma\" content=\"no-cache\">\n");
  fprintf(table_fp, "  <meta content=\"text/html; charset=UTF-8\"\n");
  fprintf(table_fp, " http-equiv=\"content-type\">\n");
  fprintf(table_fp, "  <title>Table de Trenes</title>\n");
  fprintf(table_fp, "</head>\n");
  fprintf(table_fp, "<body style=\"color: rgb(0, 0, 0); background-color: rgb(153, 153, 153);\"\n");
  fprintf(table_fp, " alink=\"#000099\" link=\"#000099\" vlink=\"#990099\">\n");
  fprintf(table_fp, "<table\n");
  fprintf(table_fp, " style=\"background-color: rgb(192, 192, 192); width: 100%%; text-align: left;\"\n");
  fprintf(table_fp, " border=\"2\" cellpadding=\"2\" cellspacing=\"2\">\n");
  fprintf(table_fp, "  <tbody>\n");
  fprintf(table_fp, "    <tr>\n");
  fprintf(table_fp, "      <td colspan=\"8\" style=\"vertical-align: top;\"><big><big><big>Linea\n");
  char buf1[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf1, sizeof(buf1), "%Y-%m-%d %T", &mytm);
  fprintf(table_fp, "1 (%s)</big></big></big><br>\n", buf1);
  fprintf(table_fp, "      </td>\n");
  fprintf(table_fp, "    </tr>\n");
  fprintf(table_fp, "    <tr>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Tren ID<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Tiempo<br>\n");
  fprintf(table_fp, "en servicio<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Tarde<br>\n");
  fprintf(table_fp, "segundos<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Porciento<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Tiempo a <br>\n");
  fprintf(table_fp, "Siguente<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Ultimo\n");
  fprintf(table_fp, "Salida<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Timpo <br>\n");
  fprintf(table_fp, "desde Salida<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "      <th style=\"vertical-align: top;\">Numero de<br>Pasajeros<br>\n");
  fprintf(table_fp, "      </th>\n");
  fprintf(table_fp, "    </tr>\n");
  fprintf(table_fp, "    <tr>\n");

  for (int i=0; i < n_trains; i++)
  {
    fprintf(table_fp, "      <td style=\"vertical-align: top;\">%s<br>\n", trains[i].train_id);
    fprintf(table_fp, "      </td>\n");
    int secs_in_service = now - trains[i].service_entry_time;
    int mins_in_service = secs_in_service / 60;
    secs_in_service %= 60;
    fprintf(table_fp, "      <td style=\"vertical-align: top;\">%02d:%02d<br>\n", mins_in_service, secs_in_service);
    fprintf(table_fp, "      </td>\n");
    fprintf(table_fp, "      <td style=\"vertical-align: top;\">%+d<br>\n", 
                     -calcs[i].seconds_late);
    fprintf(table_fp, "      </td>\n");
    if (double(calcs[i].arival_time - trains[i].service_entry_time) > 60)
    {
      double percent =
            (double(calcs[i].seconds_late) / double(calcs[i].arival_time - trains[i].service_entry_time)) * 100.0;
      fprintf(table_fp, "      <td style=\"vertical-align: top;\">%+0.1lf%%<br>\n", -percent);
    }
    else
    {
      // Ok, not enough time in service to calculate any %, just put a dash
      fprintf(table_fp, "      <td style=\"vertical-align: top;\">-<br>\n");
    }
    fprintf(table_fp, "      </td>\n");
    int secs_to_next_train =calcs[i].seconds_to_next_train;
    int mins_to_next_train = secs_to_next_train / 60;
    secs_to_next_train %= 60;
    if (i < (n_trains - 1))
    {

      fprintf(table_fp, "      <td style=\"vertical-align: top;");
      if ( calcs[i].seconds_to_next_train < 180)
      {
         fprintf(table_fp, "background-color: rgb(255, 50, 50);");
      }
      else if ( calcs[i].seconds_to_next_train < 240)
      {
         fprintf(table_fp, "background-color: rgb(255, 255, 50);");
      }

      fprintf(table_fp, "\">%02d:%02d<br>\n", 
                    mins_to_next_train, secs_to_next_train);
    }
    else
    {
      fprintf(table_fp, "      <td style=\"vertical-align: top;\">-<br>\n");
    }
    fprintf(table_fp, "      </td>\n");
    unsigned int tmp_section = n_sections / 2;
    if (trains[i].section >= tmp_section)
    {
      fprintf(table_fp, "      <td style=\"vertical-align: top; background-color: rgb(153, 153, 255);\">(%s)<br>\n",
              sections.get_station_name(trains[i].section));
    }
    else
    {
      fprintf(table_fp, "      <td style=\"vertical-align: top;\">%s<br>\n", 
             sections.get_station_name(trains[i].section));
    }
    fprintf(table_fp, "      </td>\n");

    if (calcs[i].seconds_in_section == 0)
    {
      fprintf(table_fp, " <td style=\"vertical-align: top; background-color: rgb(255, 204, 255);\">(en estaci&oacute;n)<br>\n");
    }
    else
    {
      fprintf(table_fp, "      <td style=\"vertical-align: top;\">%d<br>\n", calcs[i].seconds_in_section);
    }

    fprintf(table_fp, "      </td>\n");
    fprintf(table_fp, "      <td style=\"vertical-align: top;\">%d<br>\n",
           n_pass[trains[i].section]);
    fprintf(table_fp, "      </td>\n");
    fprintf(table_fp, "    </tr>\n");
  }

  fprintf(table_fp, "  </tbody>\n");
  fprintf(table_fp, "</table>\n");
  fprintf(table_fp, "<br>\n");
  fprintf(table_fp, "</body>\n");
  fprintf(table_fp, "</html>\n");
}

/*********************************************************/
void gen_displays_t::gen(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains)
{
  long bytes;


  for (int i=0; i < n_displays; i++)
  {
    ddata[i].gen_html(now, trains, calcs, n_trains);
  }

  fseek(table_fp, 0, SEEK_SET);
  lockf(table_fd, F_LOCK, 0);
  gen_table(now, trains, calcs, n_trains);
  fflush(table_fp);
  bytes = ftell(table_fp);
  ftruncate(table_fd, bytes);
  fseek(table_fp, 0, SEEK_SET);
  lockf(table_fd, F_ULOCK, 0);

  fseek(perf_fp, 0, SEEK_SET);
  lockf(perf_fd, F_LOCK, 0);
  gen_performance(now, trains, calcs, n_trains);
  fflush(perf_fp);
  bytes = ftell(perf_fp);
  ftruncate(perf_fd, bytes);
  fseek(perf_fp, 0, SEEK_SET);
  lockf(perf_fd, F_ULOCK, 0);
}

/*********************************************************/

void display_data_t::set(const char *a_background, const char *a_square, const char *a_unexpected,
    const char *a_fname, int a_x1, int a_x2, int a_y1, int a_y2,
     int a_text_offset_y1, int a_text_offset_y2, int n_sections)
{
  printf("creating diaplay_data_t: %s\n", a_fname);
  background = a_background;
  square = a_square;
  square_unexpected = a_unexpected;
  text_offset_y1 = a_text_offset_y1;
  text_offset_y2 = a_text_offset_y2;
  char path[300];
  const char *html_home = ap_config.get_config("HTML_HOME");
  if (html_home == NULL)
  {
    html_home = "./";
  }
  snprintf(path, sizeof(path), "%s/%s", html_home, a_fname);
  fname = strdup(path);

  printf("Opening the file for output: %s\n", fname);
  fp = fopen(fname, "w+");
  if (fp == NULL)
  {
    perror(fname);
    exit(0);
  }
  fd = fileno(fp);
  fseek(fp, 0, SEEK_SET);


  dd = new display_dist_t(a_x1, a_x2, a_y1, a_y2, n_sections);
}

/*******************************************************************/

void display_data_t::gen_html(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains) 
{
  int top = 50;
  int left = 0;

  fseek(fp, 0, SEEK_SET);
  lockf(fd, F_LOCK, 0);

  fprintf(fp, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
  fprintf(fp, "<html>\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "<META HTTP-EQUIV=\"refresh\" CONTENT=\"1\">\n");
  fprintf(fp, "<meta http-equiv=\"pragma\" content=\"no-cache\">\n");
  fprintf(fp, "  <meta http-equiv=\"content-type\"\n");
  fprintf(fp, " content=\"text/html; charset=UTF-8\">\n");
  fprintf(fp, "  <title>Linea 1</title>\n");
  fprintf(fp, "</head>\n");
  fprintf(fp, "<body style=\"color: rgb(0, 0, 0); background-color: rgb(204, 204, 204);\"\n");
  fprintf(fp, " link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">\n");
  char buf1[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf1, sizeof(buf1), "%Y-%m-%d %T", &mytm);
  fprintf(fp, "<h1>XXX-Linea 1 (%s)</h1>\n", buf1);
  fprintf(fp, "\n");
  fprintf(fp, "\n");
  fprintf(fp, "<div style=\"position:absolute; top:%dpx; left:%dpx;\">\n", top, left);
  fprintf(fp, "        <img SRC=%s>\n", background);
  fprintf(fp, "</div>\n");

  for (int i=0; i < n_trains; i++)
  {
    int tx, ty;
    bool coming_back;
    dd->calc_xy(trains[i].section, 
           calcs[i].fraction_of_section_traveled, &tx, &ty, &coming_back);
    fprintf(fp, "\n");
    fprintf(fp, "<div style=\"position:absolute; top:%dpx; left:%dpx; z-index:2\">\n",top + ty,left + tx);

    if (calcs[i].unexpected)
    {
      fprintf(fp, "        <img SRC=%s>\n", square_unexpected);
    }
    else
    {
      fprintf(fp, "        <img SRC=%s>\n", square);
    }
    fprintf(fp, "</div>\n");

    int y_diff_for_text;
    if (coming_back)
    {
      if (!calcs[i].switched_direction)
      {
        switch_direction(&trains[i], &calcs[i]);
      }
      y_diff_for_text = text_offset_y1;//-26;
      //printf("--------------------comming back: %s\n", trains[i].train_id ); 
    }
    else
    {
      y_diff_for_text = text_offset_y2;//19;
      //printf("+++++++++++++++++++NOT comming back: %s\n", trains[i].train_id ); 
    }

    fprintf(fp, "<div style=\"position:absolute; top:%dpx; left:%dpx; z-index:2\">\n",
                      top + ty + y_diff_for_text,left + tx - 15);
    fprintf(fp, "<div style=\"text-align: center;\"><span style=\"font-weight: bold; color: rgb(255, 255, 0);\">%s %+d</span></div>\n", 
                 trains[i].train_id, -calcs[i].seconds_late);

    fprintf(fp, "</div>\n");
  }
  fprintf(fp, "\n");
  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");

  fflush(fp);
  long bytes = ftell(fp);
  ftruncate(fd, bytes);
  fseek(fp, 0, SEEK_SET);
  lockf(fd, F_ULOCK, 0);
}

/*******************************************************************/

void gen_displays_t::gen_performance(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains) 
{
  fprintf(perf_fp, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
  fprintf(perf_fp, "<html>\n");
  fprintf(perf_fp, "<head>\n");
  fprintf(perf_fp, "<META HTTP-EQUIV=\"refresh\" CONTENT=\"1\">\n");
  fprintf(perf_fp, "<meta http-equiv=\"pragma\" content=\"no-cache\">\n");
  fprintf(perf_fp, "  <meta content=\"text/html; charset=UTF-8\"\n");
  fprintf(perf_fp, " http-equiv=\"content-type\">\n");
  fprintf(perf_fp, "  <title>Performance Data</title>\n");
  fprintf(perf_fp, "</head>\n");
  fprintf(perf_fp, "<body style=\"color: rgb(0, 0, 0); background-color: rgb(153, 153, 153);\"\n");
  fprintf(perf_fp, " alink=\"#000099\" link=\"#000099\" vlink=\"#990099\">\n");
  fprintf(perf_fp, "<table\n");
  fprintf(perf_fp, " style=\"background-color: rgb(192, 192, 192); width: 100%%; text-align: left;\"\n");
  fprintf(perf_fp, " border=\"2\" cellpadding=\"2\" cellspacing=\"2\">\n");
  fprintf(perf_fp, "  <tbody>\n");
  fprintf(perf_fp, "    <tr>\n");
  fprintf(perf_fp, "      <td colspan=\"2\" style=\"vertical-align: top;\"><big><big><big>Line\n");
  char buf1[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf1, sizeof(buf1), "%Y-%m-%d %T", &mytm);
  fprintf(perf_fp, "1 (%s)</big></big></big><br>\n", buf1);
  fprintf(perf_fp, "      </td>\n");
  fprintf(perf_fp, "    </tr>\n");
  fprintf(perf_fp, "  <tr>");
  fprintf(perf_fp, "    <td style=\"width: 126px;\">TrainID</td>");
  fprintf(perf_fp, "    <td style=\"width: 837px;\">Message</td>");
  fprintf(perf_fp, "  </tr>");
  int n_printed = 0;
  for (int i=0; (i < n_trains) || (n_printed < 5); i++)
  {
    if (i < n_trains)
    {
      fprintf(perf_fp, "  <tr>\n");
      if (calcs[i].seconds_late > 20)
      {
        n_printed++;
        fprintf(perf_fp, "      <td style=\"vertical-align: top;\">%s\n", trains[i].train_id);
        fprintf(perf_fp, "      </td>\n");
        fprintf(perf_fp, "      <td style=\"vertical-align: top;\">This train is %d seconds late tell him to hurry\n", 
                                  calcs[i].seconds_late);
        fprintf(perf_fp, "      </td>\n");
      }
      if (calcs[i].seconds_late < -20)
      {
        n_printed++;
        fprintf(perf_fp, "      <td style=\"vertical-align: top;\">%s<br>\n", trains[i].train_id);
        fprintf(perf_fp, "      </td>\n");
        fprintf(perf_fp, "      <td style=\"vertical-align: top;\">This train is early tell him to wait %d seconds at the next station\n", 
                  -calcs[i].seconds_late);
        fprintf(perf_fp, "      </td>\n");
      }
    }
    else
    {
      n_printed++;
      fprintf(perf_fp, "      <td style=\"vertical-align: top;\">|               \n");
      fprintf(perf_fp, "      </td>\n");
      fprintf(perf_fp, "      <td style=\"vertical-align: top;\">|                              \n");
      fprintf(perf_fp, "      </td>\n");
    }
    fprintf(perf_fp, "  </tr>\n");
  }
  fprintf(perf_fp, "  </tbody>\n");
  fprintf(perf_fp, "</table>\n");
  fprintf(perf_fp, "<br>\n");
  fprintf(perf_fp, "</body>\n");
  fprintf(perf_fp, "</html>\n");
} 


/*******************************************************************


int main(int argc, char *argv[])
{
}

*******************************************************************/
