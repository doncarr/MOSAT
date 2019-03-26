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

#ifndef __LOGFILE_INC__
#define __LOGFILE_INC__

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#define START "/home/croma/public_html/experimentos"
#define DIR_MODES (S_IRUSR | S_IWUSR | S_IXUSR | S_IXGRP |\
	       	S_IXOTH |S_IRGRP | S_IROTH)
//#define dir_exists(f) (0 == access(f,F_OK))
//#define file_exists(f) (0 == access(f,F_OK))

class logfile_t
{
private:
  FILE *fp;
  FILE *fp_startup;
  const char *loghome;
  char path_to_dir[500];
  char path_to_day[500];
  int last_test_number;
  char exp_num[30];
  bool file_print_enabled;
  bool screen_print_enabled;
  bool startup_print_enabled;
  void  previous_day(void);
  char *tests[100];
  int n_tests;

public:
  logfile_t(void);
  logfile_t(bool print_to_file, bool print_to_screen);
  void do_init(bool print_to_file, bool print_to_screen);
  void create_dir(void);
  void close(void);
  void write(const char *txt);
  void print(const char *txt);
  void perror(const char *txt);
  void vprint(const char *fmt, ...)
#ifdef __GNUC__
          __attribute__ (( format( printf, 2, 3 ) ));
#endif 
  FILE *open_file_in_log_dir(const char *name);
  void copy_file_to_log_dir(const char *file, const char *name);
  void write_final_logs(int n);
  void insert_in_index(const char *fname, const char *secuencia);
  void next_test(void);
  void close_test(void);
  void create_graph(void);
  void set_startup_log_on(void);
  void set_startup_log_off(void);
  //void screen_print_enabled(bool v);
};

extern logfile_t *logfile;
 

/****************************************************************/
#endif
