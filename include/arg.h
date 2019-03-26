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


/*************************************************************************

Arg.h

Header file for get_delim_args().

*************************************************************************/

#ifndef __ARG_INC__
#define __ARG_INC__

class delim_file_t
{
private:
  FILE *fp;
  char *line;
  char *tmp;
  int max_line_size;
  int max_args;
  int line_num;
  char **argv;
  char delim;
  char comment;
  bool do_print_lines;
public:
  delim_file_t(int a_max_line_size, int a_max_args,
               char a_delim, char a_comment);
  ~delim_file_t(void);
  char** first(const char *fname, int *n_args, int *a_line_num);
  char** next(int *n_args, int *a_line_num);
  char *get_last_line(void);
  void print_lines(bool plines) {do_print_lines = plines;};
};

class delim_separator_t
{
private:
  FILE *fp;
  char *line;
  char *tmp;
  int max_line_size;
  int max_args;
  char **argv;
  char delim;
public:
  delim_separator_t(int a_max_line_size, int a_max_args, char a_delim);
  ~delim_separator_t(void);
  char** first(const char *fname, int *n_args, int *a_line_num);
  char** separate(int *n_args, const char *line);
};



int get_delim_args(char *line, char *argv[], char delimeter, int max_args);
int get_delim_array(char *line, char *argv[], char delimeter, char left_char, char right_char, int max_args);

#endif

