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

/***********
Warning bak up files before using this program, it only works 
if the existing license is at the beginning of the file and formatted
in a very specific way. Be very carefull
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtcommon.h"

int main(int argc, char *argv[])
{
  for (int i=1; i < argc; i++)
  {
    char fnamebak[200];
    char fnamesrc[200];
    snprintf(fnamebak, sizeof(fnamebak), "%s.lbak", argv[i]);
    snprintf(fnamesrc, sizeof(fnamesrc), "%s", argv[i]);
    char cmd[300];
    printf("argv[%d] = %s\n", i, argv[i]);
    snprintf(cmd, sizeof(cmd), "mv %s %s.lbak", argv[i], argv[i]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "cp license.txt %s", argv[i]);
    system(cmd);
    //snprintf(cmd, sizeof(cmd), "cat %s.lbak >> %s", argv[i], argv[i]);
    //system(cmd);

    FILE *fp = fopen(fnamesrc, "a");
    if (fp == NULL)
    {
      printf("Can't open %s\n", fnamesrc);
      continue;
    }
    FILE *fpbak = fopen(fnamebak, "r");
    if (fp == NULL)
    {
      printf("Can't open %s\n", fnamebak);
      continue;
    }
    char line[300];
    bool done = false;
    printf("searching start of comment\n");
    while ((NULL != fgets(line, sizeof(line), fpbak)) && !done)
    {
      rtrim(line);
      printf("%@%@ %s\n", line);
      if (strlen(line) == 0) continue;
      if (line[0] == '#')
      {
        printf("----- hit #, no license here\n");
        break;
      }
      if (0 == strncmp(line, "/***", 4))
      {
        printf("------ found comment, searching copyright\n");
        while ((NULL != fgets(line, sizeof(line), fpbak)) && !done)
        {
          rtrim(line);
          ltrim(line);
          printf("&#&# %s\n", line);
          if (0 == strncasecmp(line, "copyright", 9))
          {
            printf("found copyright, searching end comment\n");
            while ((NULL != fgets(line, sizeof(line), fpbak)) && !done)
            {
              rtrim(line);
              printf("-*-* %s\n", line);
              if (0 == strncmp(line, "****", 4))
              {
                 printf("found end comment\n");
                 done = true;
              }
              if (done) break;
            }
            if (done) break;
          }
          if (done) break;
        }
        if (done) break;
      }
      if (done) break;
    }

    if (!done)
    {
      printf("no license found, re-winding\n");
      rewind(fpbak);
    }

    while (NULL != fgets(line, sizeof(line), fpbak))
    {
      rtrim(line);
      fprintf(fp, "%s\n", line);
    }
    fclose(fp);
    fclose(fpbak);

    snprintf(cmd, sizeof(cmd), "rm %s.lbak", argv[i]);
    system(cmd);
  }
}
