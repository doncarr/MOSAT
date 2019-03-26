/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <math.h>

#include "rtcommon.h"
#include "spd_display.h"

int select_from_list(int n, const char *list[], const char *prompt)
{
  int i = 0;
  //printf("In select from list\n");
  while (1) 
  {
    //printf("%s, %s\n", prompt, list[i]);
    int key = spd_prompt_user(list[i], prompt);
    if ((key == '#') || (key == 'm'))
    {
      //char str[40];
      //snprintf(str, sizeof(str), "You picked %d", i);
      //spd_prompt_user(str , "hit # to continue");
      return i;
    }
    if (key == 'f')
    {
      if (i <= 0)
      {
        i = n - 1;
      }
      else
      {
        i--;
      }
    }
    if (key == 's')
    {
      if (i >= (n-1))
      {
        i = 0;
      }
      else
      {
        i++;
      }
    }
    if (key == '1')
    {
      i = 0;
    }
    if (key == '2')
    {
      i = 1;
    }
    usleep(100000);
  }
}

