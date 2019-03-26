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
/******

web.cpp

Member functions for web points.

*************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "ap_config.h"

/**********************************************************************/

static int gen_image_at(char *bp, int bp_size, int top, int left, char *name)
{
  int n = snprintf(bp, bp_size, "<div style=\"position: absolute; top: %dpx; left: %dpx;\"> <img src=\"%s\">\n</div>", 
             top, left, name);
  return n;
}

/**********************************************************************/

static void gen_bar(char *bp, int bp_size, 
             char *base, char *half_on, char *half_off, 
             int top, int left, int length,
             double eu_lo, double eu_hi, double pv) 
{
  double fraction = (pv - eu_lo) / (eu_hi - eu_lo);
  int sz = gen_image_at(bp, bp_size, top, left, base);
  bp += sz;
  bp_size -= sz;
  //printf("%0.2lf ", fraction);
  if (fraction < 0.0)
  {
    fraction = 0.0;
  }
  else if (fraction > 1.0)
  {
    fraction = 1.0;
  }
  //printf("%0.2lf ", fraction);
  int pixels = int(fraction * double(length)); 
  //printf("%d ", pixels);
  if (fraction < 0.5)
  {
    int btop =  top + (length / 2) - pixels; 
    //printf("%d %d %d %d %d\n ", btop, left, top, length, pixels);
    gen_image_at(bp, bp_size, btop, left, half_off);
  }
  else 
  {
    int btop =  top + length - pixels;
    //printf("%d %d\n ", btop, left);
    gen_image_at(bp, bp_size, btop, left, half_on);
  }
}

/**********************************************************************/

void web_point_t::update(void)
{
  /* Update the web point. */

  double now = db->get_time();
  if ((now - last_time) >= update_interval)
  {
    last_time = now;
    //printf("Updating %s ..........\n", this->tag);
    this->update_file_and_write();
  }
}

/*********************************************************************/

void web_point_t::init_values(void)
{
  const char *html_home = ap_config.get_config("htmlhome");
  const char *template_home = ap_config.get_config("templatehome");
  if (html_home == NULL)
  {
    safe_strcpy(output_file, output_name, sizeof(output_file));
  }
  else
  {
    char stmp[200];
    snprintf(stmp, sizeof(stmp), "%s/%s", html_home, output_name);
    safe_strcpy(output_file, stmp, sizeof(output_file));
  }

  if (template_home == NULL)
  {
    safe_strcpy(template_file, template_name, sizeof(template_file));
  }
  else
  {
    char stmp[200];
    snprintf(stmp, sizeof(stmp), "%s/%s",template_home, template_name);
    safe_strcpy(template_file, stmp, sizeof(template_file));
  }

  pv = false;
  safe_strcpy(hi_desc, "True", sizeof(hi_desc));
  safe_strcpy(lo_desc, "False", sizeof(lo_desc));
  pv_string = lo_desc;
  last_time = 0.0;

  read_file();
}

/*********************************************************************/

void web_point_t::read_file(void)
{
  FILE *fp_html = fopen(template_file, "r");
  if (fp_html == NULL)
  {
    logfile->perror(template_file);
    logfile->vprint("Can't open file: %s\n", template_file);
    drefs = NULL;
    irefs = NULL;
    file_buf = NULL;
    file_size = 0;
    return;
  }
  struct stat buf;
  //int fstat(int filedes, struct stat *buf);
  fstat(fileno(fp_html), &buf);

  file_size = buf.st_size;
  file_buf = (char *) malloc(file_size+1);
  // size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
  fread(file_buf, file_size, 1, fp_html);
  fclose(fp_html);
  file_buf[file_size] = '\0';

  drefs = NULL;
  arefs = NULL;
  irefs = NULL;

  for (int i=0; i < file_size; i++)
  {
    if (0 == strncasecmp(&file_buf[i], "$$$$TIME(", 9))
    {
      time_location = &file_buf[i];
      int j;
      for (j = i; file_buf[j] != ')'; j++);
      time_size = j - i + 1;
      //char *time_location;
      //int time_size;
    }
    else if (0 == strncasecmp(&file_buf[i], "$$$$INT(", 8))
    {
      int_ref_t *newiref = new int_ref_t;
      newiref->next = irefs;
      irefs = newiref;
      newiref->str = &file_buf[i];
      int j;
      for (j = i; file_buf[j] != ')'; j++);
      newiref->size = j - i + 1;
      int len = newiref->size-8;
      char *tmp = new char[len+1];
      newiref->arg_str = tmp; // Save so we can delete it later.
      strncpy(tmp, file_buf + i + 8, len);
      tmp[len] = '\0';
      int argc;
      char *argv[30];
      argc = get_delim_args(tmp, argv, '|', 30);
      for (int m=0; m < argc; m++)
      {
        logfile->vprint("argv[%d]: %s\n", m, argv[m]);
      }
      rtrim(argv[0]);
      ltrim(argv[0]);
      rtrim(argv[1]);
      ltrim(argv[1]);
      db_point_t *db_point;
      db_point = db->get_db_point(argv[0]);
      if (db_point == NULL)
      {
        newiref->ip = NULL;
        newiref->nvals = 0;
        logfile->vprint("%s - NOT a database point: %s\n", template_file, argv[0]);
        continue;
      }
      else if (db_point->pv_type() != INTEGER_VALUE)
      {
        newiref->ip = NULL;
        newiref->nvals = 0;
        logfile->vprint("%s - NOT an integer point: %s\n", template_file, argv[0]);
        continue;
      }
      else
      {
        newiref->ip = (int_t *) db_point;
      }
      newiref->default_str = argv[1];
      newiref->nvals = 0;
      for (int k=2; k < (argc - 1); k += 2)
      {
        if (newiref->nvals >= 
             int(sizeof(newiref->strings) / sizeof(newiref->strings[0])))
        {
          logfile->vprint("***** Too many values for integer ref\n");
          break;
        }
        rtrim(argv[k]);
        ltrim(argv[k]);
        rtrim(argv[k+1]);
        ltrim(argv[k+1]);
        if (argv[k+1][strlen(argv[k+1])-1] == ')')
        {
          argv[k+1][strlen(argv[k+1])-1] = '\0';
        }
        rtrim(argv[k+1]);
        newiref->values[newiref->nvals] = atol(argv[k]);
        newiref->strings[newiref->nvals] = argv[k+1];
        newiref->nvals++;
      }

      logfile->vprint("Integer point: %s, default value: %s\n",
           newiref->ip->get_tag(), newiref->default_str);
      for (int k=0; k < newiref->nvals; k++)
      {
        logfile->vprint("Value: %d, string: %s\n",
            newiref->values[k], newiref->strings[k]);
      }
    }
    else if (0 == strncasecmp(&file_buf[i], "$$$$DI(", 7))
    {
      discrete_ref_t *tmp_r = new discrete_ref_t;
      tmp_r->next = drefs;
      drefs = tmp_r;
      tmp_r->str = &file_buf[i];
      int j;
      for (j = i; file_buf[j] != ')'; j++);
      tmp_r->size = j - i + 1;
      int len = tmp_r->size-7;
      char *tmp = new char[len+1];
      tmp_r->arg_str = tmp; // Save so you can delete it
      strncpy(tmp, file_buf + i + 7, len);
      tmp[len] = '\0';
      int argc;
      char *argv[10];
      argc = get_delim_args(tmp, argv, '|', 10);
      if (argc == 3)
      {
        rtrim(argv[0]);
        ltrim(argv[0]);
        rtrim(argv[1]);
        ltrim(argv[1]);
        rtrim(argv[2]);
        ltrim(argv[2]);
        if (argv[2][strlen(argv[2])-1] == ')')
        {
          argv[2][strlen(argv[2])-1] = '\0';
        }
        rtrim(argv[2]);
        tmp_r->true_string = argv[1];
        tmp_r->false_string = argv[2];
        //tmp_r->dp = NULL;

        db_point_t *db_point;
        db_point = db->get_db_point(argv[0]);
        if (db_point == NULL)
        {
          tmp_r->dp = NULL;
          logfile->vprint("%s - NOT a database point: %s\n", template_file, argv[0]);
        }
        else if (db_point->pv_type() != DISCRETE_VALUE)
        {
          tmp_r->dp = NULL;
          logfile->vprint("%s - NOT a discrete point: %s\n", template_file, argv[0]);
        }
        else
        {
          logfile->vprint("    Adding Discrete value: %s,  strings: %s %s\n", argv[0], argv[1], argv[2]);
          tmp_r->dp = (discrete_point_t *) db_point;
        }
      }
      else
      {
        logfile->vprint("Wrong number of args for discrete $$$$DI: %d\n", argc);
        tmp_r->true_string = "*** Wrong number of args ***";
        tmp_r->false_string = "*** Wrong number of args ***";
        tmp_r->dp = NULL;
      }
    }
    else if (0 == strncasecmp(&file_buf[i], "$$$$AI(", 7))
    {
      analog_ref_t *tmp_r = new analog_ref_t;
      tmp_r->next = arefs;
      arefs = tmp_r;
      tmp_r->str = &file_buf[i];
      tmp_r->is_bar = false;
      int j;
      for (j = i; file_buf[j] != ')'; j++);
      tmp_r->size = j - i + 1;
      int len = tmp_r->size-7;
      char *tmp = new char[len+1];
      tmp_r->arg_str = tmp; // Save so it can be deleted 
      strncpy(tmp, file_buf + i + 7, len);
      tmp[len] = '\0';
      int argc;
      char *argv[10];
      logfile->vprint("%s\n", tmp);
      argc = get_delim_args(tmp, argv, '|', 10);
      logfile->vprint("Args for AI = %d\n", argc);
      if ((argc == 1) || (argc == 4) || (argc == 9))
      {
        rtrim(argv[0]);
        ltrim(argv[0]);
        if (argv[argc-1][strlen(argv[argc-1])-1] == ')')
        {
          argv[argc-1][strlen(argv[argc-1])-1] = '\0';
        }
        rtrim(argv[0]);

        db_point_t *db_point;
        db_point = db->get_db_point(argv[0]);
        if (db_point == NULL)
        {
          tmp_r->ap = NULL;
          logfile->vprint("%s - NOT a database point: %s\n", template_file, argv[0]);
        }
        else if (db_point->pv_type() != ANALOG_VALUE)
        {
          tmp_r->ap = NULL;
          logfile->vprint("%s - NOT an analog point: %s\n", template_file, argv[0]);
        }
        else
        {
          tmp_r->ap = (analog_point_t *) db_point;
          if (argc == 4)
          {
            rtrim(argv[1]);
            ltrim(argv[1]);
            rtrim(argv[2]);
            ltrim(argv[2]);
            rtrim(argv[3]);
            ltrim(argv[3]);
            tmp_r->bar = argv[1];
            tmp_r->half_off = argv[2];
            tmp_r->half_on = argv[3];
            tmp_r->bar_top = 30;
            tmp_r->bar_left = 710;
            tmp_r->bar_length = 400;
            tmp_r->bar_min = 0;
            tmp_r->bar_max = 500;
            tmp_r->is_bar = true;
            logfile->vprint("-- 4 parameter bar\n");
            logfile->vprint("Bar: %s, %s, %s\n", tmp_r->bar, tmp_r->half_off, tmp_r->half_on);
          }
          else if (argc == 9)
          {
            rtrim(argv[1]);
            ltrim(argv[1]);
            rtrim(argv[2]);
            ltrim(argv[2]);
            rtrim(argv[3]);
            ltrim(argv[3]);
            tmp_r->bar = argv[1];
            tmp_r->half_off = argv[2];
            tmp_r->half_on = argv[3];
            tmp_r->bar_top = atol(argv[4]);
            tmp_r->bar_left = atol(argv[5]);
            tmp_r->bar_length = atol(argv[6]);
            tmp_r->bar_min = atol(argv[7]); 
            tmp_r->bar_max = atol(argv[8]);
            tmp_r->is_bar = true;
            logfile->vprint("-- 9 parameter bar\n");
            logfile->vprint("Bar: %s, %s, %s, %d, %d, %d, %d, %d\n", 
                tmp_r->bar, tmp_r->half_off, tmp_r->half_on,
                tmp_r->bar_top, tmp_r->bar_left, tmp_r->bar_length, tmp_r->bar_min, tmp_r->bar_max);
          }
        }
      }
      else
      {
        logfile->vprint("Wrong number of args for $$$$AI: %d\n", argc);
        tmp_r->ap = NULL;
      }
    }
  }
}

/********************************************************************/

void web_point_t::subst_string(char *sdest, const char *snew, int len)
{
  /* This function copies at most len characters into sdest from snew, and then space pads */
  bool end = false;
  for (int i=0; i < len; i++)
  {
    if (end)
    {
      /* ok, space pad if we have extra space */
      sdest[i] = ' ';
      continue;
    }
    if (snew[i] == '\0')
    {
      end = true;
      sdest[i] = ' ';
      continue;
    }
    sdest[i] = snew[i];
  }
}

/***
static bool test[12] =
  {false, false, false, false,false, false, false, false,false, false, false, false};
static int tcount = 0;
***/

/********************************************************************/

web_point_t::~web_point_t(void)
{
  this->free_all();
}

/********************************************************************/

void web_point_t::free_all(void)
{
  // This frees all of the data allocated for a web point, so that it can be re-loaded;
  int_ref_t *ir, *inext;
  ir = irefs;
  while (ir != NULL)
  {
    inext = ir->next;
    if (ir->arg_str != NULL) delete ir->arg_str;
    delete ir;
    ir = inext;
  }
  irefs = NULL;

  discrete_ref_t *dr, *dnext;
  dr = drefs;
  while (dr != NULL)
  {
    dnext = dr->next;
    if (dr->arg_str != NULL) delete dr->arg_str;
    delete dr;
    dr = dnext; 
  }
  drefs = NULL;

  analog_ref_t *ar, *anext;
  ar = arefs;
  while (ar != NULL)
  {
    anext = ar->next;
    if (ar->arg_str != NULL) delete ar->arg_str;
    delete ar;
    ar = anext;
  }
  arefs = NULL;
}

/********************************************************************/

void web_point_t::update_file_and_write(void)
{

  if (time_location != NULL)
  {
    char datestr[100];
    time_t t;
    t = time(NULL);
    struct tm *mytm;
    mytm = localtime(&t);
    strftime(datestr, sizeof(datestr), "%Y-%m-%d %H:%M:%S%z", mytm);
    //strftime(timestr, sizeof(timestr), "%T", mytm);

    subst_string(time_location, datestr, time_size);
  }

  int_ref_t *ir;
  ir = irefs;
  while (ir != NULL)
  {
    int intval = 0;
    if (ir->ip != NULL)
    {
      intval = ir->ip->get_pv();
    }
    else
    {
      intval = 0;
    }
    char *src = NULL;
    for (int i=0; i < ir->nvals; i++)
    {
      if (intval == ir->values[i])
      {
        src = ir->strings[i];
      }
    }
    if (src == NULL)
    {
      src = ir->default_str;
    }
    subst_string(ir->str, src, ir->size);
    ir = ir->next;
  }

  discrete_ref_t *dr;
  dr = drefs;
  while (dr != NULL)
  {
    bool current_val;
    if (dr->dp != NULL)
    {
      //printf("val = %s\n", (current_val)?"----T----":"F");
      current_val = dr->dp->get_pv();
    }
    else
    {
      current_val = false;
    }
    const char *src;
    if (current_val)
    {
      src = dr->true_string;
    }
    else
    {
      src = dr->false_string;
    }
    subst_string(dr->str, src, dr->size);
    dr = dr->next;
  }

  analog_ref_t *ar;
  ar = arefs;
  while (ar != NULL)
  {
    double current_aval;
    if (ar->ap != NULL)
    {
      //printf("val = %s\n", (current_aval)?"----T----":"F");
      current_aval = ar->ap->get_pv();
    }
    else
    {
      current_aval = 0.0;
    }
    if (ar->is_bar)
    {
      char buf[220];
      gen_bar(buf, sizeof(buf), 
             ar->bar, ar->half_on, ar->half_off, 
             ar->bar_top, ar->bar_left, ar->bar_length,
             ar->bar_min, ar->bar_max, current_aval); 
      //printf("%s\n", buf);
      subst_string(ar->str, buf, ar->size);
    }
    else
    {
      char abuf[15];
      snprintf(abuf, sizeof(abuf), "%0.1lf", current_aval);
      subst_string(ar->str, abuf, ar->size);
    }
    
    ar = ar->next;
  }


  FILE *fp = fopen(output_file, "w");
  if (fp == NULL)
  {
    logfile->perror(output_file);
    logfile->vprint("Can't open file: %s\n", output_file);
    return;
  }
  fwrite(file_buf, file_size, 1, fp);
  fclose(fp);
}

/********************************************************************/

