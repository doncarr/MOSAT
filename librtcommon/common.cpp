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


/***********************************************************************

Common.c

Contains procedures used by almost all programs.

**********************************************************************/

#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rtcommon.h"


/*********************************************************************/

bool is_a_space(char ch)
{
  return (ch == '\n') || (ch == '\r') || (ch == ' ') || (ch == '\t');
}

/*********************************************************************/
void strip_quotes(char *str)
{
  int i, j;

  for (i = 0; (str[i] != '\0') && (str[i] == '\"'); i++);
  for (j = 0; (str[i] != '\0'); str[j++] = str[i++]);
  str[j] = '\0';
  for (i = (strlen(str) - 1);
	(str[i] == '\"') && (i >= 0); str[i--] = '\0');
}

/*********************************************************************/
void rtrim(char *str)
{
  /* Trim the space from the right of a string. */
  for (int i = (strlen(str) - 1);
	is_a_space(str[i]) && (i >= 0); str[i--] = '\0');
}
/*********************************************************************/

void ltrim(char *str)
{
  /* Trim the space from the left side of a string. */
  int i, j;

  for (i = 0; (str[i] != '\0') && is_a_space(str[i]); i++);
  for (j = 0; (str[i] != '\0'); str[j++] = str[i++]);
  str[j] = '\0';
}

/*********************************************************************/

char *find_str(char *place_to_look, const char *string_to_find)
{
  char *p;
  int n = strlen(string_to_find);
  for (p = place_to_look; *p != '\0'; p++)
  {
    if  (0 == strncmp(p, string_to_find, n))
    {
      return p;
    }
  }
  return NULL;
}

/*********************************************************************/

