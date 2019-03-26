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
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "db_point.h"
#include "db.h"
#include "rtcommon.h"
#include "arg.h"
#include "reactpoint.h"

struct discrete_ref_t
{
  char *str;
  int size;
  char *true_string;
  char *false_string;
  discrete_point_t *dp;
  discrete_ref_t *next;
};

class gen_html_t
{
private:
  int file_size;
  char *file_buf;
  discrete_ref_t *drefs;
  void subst_string(char *sdest, char *snew, int len);
  char *output_file;
public:
  void read_file(char *html_file, char *an_output_file);
  void update_file_and_write(void);
};

/*******************************************************************/

void gen_html_t::read_file(char *html_file, char *an_output_file)
{
  FILE *fp_html = fopen(html_file, "r");
  if (fp_html == NULL)
  {
    printf("Can't open file: %s\n", html_file);
    drefs = NULL;
    file_buf = NULL;
    file_size = 0;
    return;
  }
  this->output_file  = strdup(an_output_file);

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
  for (int i=0; i < file_size; i++)
  {
    if (0 == strncasecmp(&file_buf[i], "$$$$DI(", 7))
    {
      discrete_ref_t *r = new discrete_ref_t;
      r->next = drefs;
      drefs = r;
      r->str = &file_buf[i];
      int j;
      for (j = i; file_buf[j] != ')'; j++);
      r->size = j - i + 1;
      int len = r->size-7;
      char *tmp = new char[len+1];
      strncpy(tmp, file_buf + i + 7, len);
      tmp[len] = '\0';
      printf("%s, %d\n", tmp, r->size);
      int argc;
      char *argv[10];
      argc = get_delim_args(tmp, argv, '|', 10);
      if (argc == 3)
      {
        rtrim(argv[1]);
        ltrim(argv[1]);
        rtrim(argv[2]);
        ltrim(argv[2]);
        if (argv[2][strlen(argv[2])-1] == ')')
        {
          argv[2][strlen(argv[2])-1] = '\0';
        }
        rtrim(argv[2]);
        r->true_string = argv[1];
        r->false_string = argv[2];
        r->dp = NULL;
      }
      else
      {
        r->true_string = "255,255,153";
        r->false_string = "192,192,192";
        r->dp = NULL;
      }
    }
  }
}

/********************************************************************/

void gen_html_t::subst_string(char *sdest, char *snew, int len)
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

static bool test[12] =
  {false, false, false, false,false, false, false, false,false, false, false, false};
static int tcount = 0;

/********************************************************************/

void gen_html_t::update_file_and_write(void)
{
  discrete_ref_t *dr;
  dr = drefs;

  while (dr != NULL)
  {
    bool val = test[tcount];//dr->dp->get_pv();
    tcount = (tcount + 1) % 12;
    char *src;
    if (val)
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
  FILE *fp = fopen(output_file, "w");
  if (fp == NULL)
  {
    printf("Can't open file: %s\n", output_file);
    return;
  }
  fwrite(file_buf, file_size, 1, fp);
  fclose(fp);
}

/********************************************************************/

int main(int argc, char *argv[])
{
  tcount = 0;
  printf("argc = %d\n", argc);
  for (int i=1; i < argc; i++)
  {
    int n = atol(argv[i]);
    printf("n = %d\n", n);
    n--;
    n = 11 - n;
    test[n%12] = true;
  }
  gen_html_t g;
  printf("Reading files\n");
  g.read_file("gentest/Subestaciones.html", "gentest/test.tags", "gentest/gen.html");
  printf("update and write file\n");
  g.update_file_and_write();
}

