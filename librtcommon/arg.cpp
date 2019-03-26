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

Arg.c

Procedure to get delimited arguments.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "rtcommon.h"
#include "arg.h"


/***********************************************************************/
delim_file_t::~delim_file_t(void)
{
  //printf("Deleting delim_file_t\n");
  if (fp != NULL)
  {
    fclose(fp);
  }
  if (line != NULL)
  {
    free(line);
  }
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

delim_file_t::delim_file_t(int a_max_line_size, 
                           int a_max_args, 
                           char a_delim, char a_comment)
{
  //printf("Creating delim_file_t\n");
  fp = NULL;
  max_line_size = a_max_line_size;
  max_args = a_max_args;
  delim = a_delim;
  comment = a_comment;
  line = (char *) malloc(max_line_size);
  
  tmp = (char *) malloc(max_line_size);
  argv = (char **) malloc(max_args * sizeof(char*));
  line_num = 0;
  do_print_lines = false;
}

/***********************************************************************/

char **delim_file_t::first(const char *fname, int *n_args, int *a_line_num)
{
  if (fp != NULL)
  {
    //printf("Closing previous file\n");
    fclose(fp);
  }
  //printf("Opening file: %s\n", fname);
  fp = fopen(fname, "r");
  if (fp == NULL)
  {
    *n_args = 0;
    *a_line_num = 0;
    perror(fname);
    return NULL;
  }
  line_num = 0;
  //printf("File opened, getting first args %p\n", fp);
  return next(n_args, a_line_num);
}

/***********************************************************************/

char **delim_file_t::next(int *n_args, int *a_line_num)
{
  //printf("Getting args %p\n", fp);
  for (int i=0; NULL != fgets(line, max_line_size, fp); i++)
  {
    int argc;

    line_num++;
    if (do_print_lines)
    {
    }
    safe_strcpy(tmp, line, max_line_size);
    //printf("%s", tmp);
    argc = get_delim_args(tmp, argv, delim, max_args);
    if (argc == 0)
    {
      continue;
   }
    else if (argv[0][0] == comment)
    {
      continue;
    }
    *n_args = argc;
    *a_line_num = line_num;
    return argv;
  }
  return NULL;

}

/***********************************************************************/
char *delim_file_t::get_last_line(void)
{
  return line;
}

int get_delim_args(char *line, char *argv[], char delimeter, int max_args)

  /*
   * This function splits the line into delimeter seperated arguments and
   * returns the number of arguments found.  There MUST be a delimeter
   * after the last argument. Leading and trailing spaces are significant.
   *
   * Warning: the string passed in is modified.
   */
{
  int i;
  char *p;

  if (strlen(line) == 0)
  {
    return 0; 
  }
  p = line + (strlen(line) - 1);
  // Here we trim white space off of the end of the string, but do not trim
  // delimeters that might also be white space, such as a tab. 
  // And, you need to stop at the start of the string!
  while ((isspace(*p)) && (*p != delimeter) && (p > line))
  {
    *p = '\0';
     p--;
  }
  p = line;
  int n_delim = 0;
  for (i=0; i < max_args; i++)
  {
    if (*p == '\0')
    {
      break;
    }
    argv[i] = p;
    for ( ; (*p != '\0') && (*p != delimeter); p++);
    if (*p == delimeter)
    {
      n_delim++;
      *(p++) = '\0';
    }
    else
    {
      continue;
    }
  }
  bool last_arg_ok = false;
  for (p = argv[i-1]; *p != '\0'; p++)
  {
    if (!isspace(*p))
    {
      last_arg_ok = true;
      break;
    }
  }
  // This is a little tricky. It is possible the last arg is blank, but
  // in that case, there must be a trailing delimeter. If there is just
  // white space after the last delimiter, then it is not considered
  // to be another argument.
  if (!last_arg_ok && (n_delim < i))
  {
    i--;
  }
  return i;
}

/***********************************************************************/

int get_delim_array(char *line, char *argv[], char delimeter, char left_char, char right_char, int max_args)
{
  // This assumes that there is an array in the string with a left array character and a right array character, typically [] or {}
  // This also works to separate the elements of a structure.
  char *p;  
  char *start = NULL;
  bool found = false;
  for (p=line; *p != '\0'; p++)
  {
    if (*p == left_char)
    {
      *p = '\0';
      found = true;
      start = p + 1;
    }
  }
  if (not found) return 0;

  found = false;
  int len = strlen(start);
  for (p = start + (len - 1); p >= start; p--)
  {
    if (*p == right_char)
    {
      *p = '\0';
      found = true;
    }
  }
  if (not found) return 0;
  
  return get_delim_args(start, argv, delimeter, max_args);
}
