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

/**************************************************************************

separator.cpp

Procedure to get delimited arguments for a single line.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "rtcommon.h"
#include "arg.h"


/***********************************************************************/
delim_separator_t::~delim_separator_t(void)
{
  //printf("Deleting delim_separator_t\n");
  if (tmp != NULL)
  {
    free(tmp);
  }
  if (argv != NULL)
  {
    free(argv);
  }
}

/***********************************************************************/

delim_separator_t::delim_separator_t(int a_max_line_size, 
                           int a_max_args, 
                           char a_delim)
{
  //printf("Creating delim_separator_t\n");
  max_line_size = a_max_line_size;
  max_args = a_max_args;
  delim = a_delim;
  tmp = (char *) malloc(max_line_size);
  argv = (char **) malloc(max_args * sizeof(char*));
}

/***********************************************************************/

char **delim_separator_t::separate(int *n_args, const char *line)
{
  safe_strcpy(tmp, line, max_line_size);
  int argc;
  argc = get_delim_args(tmp, argv, delim, max_args);
  *n_args = argc;
  return argv;
}

/***********************************************************************/

