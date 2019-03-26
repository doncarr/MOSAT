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
#include <stdio.h>
#include <stdlib.h>

//#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>


#include "db.h"
#include "logfile.h"
#include "conio.h"
#include "ap_config.h"
#include "timeaccum.h"
#include "utimer.h"

react_base_t *db = NULL;
static react_t *reactdb = NULL;

logfile_t *logfile = NULL;
ap_config_t ap_config;
bool signal_recieved = false;

/*****************************************************************/


int num_tests(char *name)
{
  int end = strlen(name) - 5;
  if (!isdigit(name[end]))
  {
    return 0;
  }
  int i;
  for (i=end; isdigit(name[i]) && (i > 0); i--);
  i++;
  return atol(&name[i]);
}

/*****************************************************************/

void edit_files(const char *base, int n)
{
  if (n == 0)
  {
    return;
  }
  char cmd[600];
  const char *the_editor = ap_config.get_config("editor");
  if (the_editor == NULL)
  {
    logfile->vprint("No editor defined, using gedit\n");
    the_editor = "gedit --new-window";
  }

  safe_strcpy(cmd, the_editor, sizeof(cmd));
  for (int i=1; i <= n; i++)
  {
    char tmp[50];
    snprintf(tmp, sizeof(tmp), " %s%d.txt", base, i);
    safe_strcat(cmd, tmp, sizeof(cmd));
  }
  system(cmd);
}


/*****************************************************************/
void my_sighandler(int signum)
{
  if (signum == SIGIO) return; 
  printf("Got a signal: %d\n", signum);
  if (SIGIO == signum) return;
  exit(0);
}
/*****************************************************************/
void react_signal_handler(int sig_num)
{
  signal_recieved = true;
}


/*****************************************************************/

void exit_clean_up(void)
{
  reactdb->print_all_points();
  if (logfile != NULL)
  {
    logfile->close();
  }
  if (reactdb != NULL)
  {
    reactdb->exit_clean_up();
  }
  coniorestore();
  if (!signal_recieved) // Ok, if we are exiting because of a signal, do NOT prompt.
  {
    printf("End of program. Hit <enter> to exit . . .");
    char buf[10];
    fgets(buf, 10, stdin);
  }
}

/*****************************************************************/

void print_help()
{
  printf("Usage: react2 [-d <home-directory>] [-x] [sequence-name]\n\n");
  printf("Run the react control engine\n\n");
  printf("  -d   use an alternate directory for the react home directory.\n");
  printf("       The -d command must be followed by a home directory.\n");
  printf("       This will be used as the home directory, in place of\n");
  printf("       the current directory. React reads files from two\n");
  printf("       directories under the home directory:\n");
  printf("            dbfiles and corridas.\n");
  printf("  -x   do NOT execute a sequence by default\n");
  printf("       if you do not use the -x option, you must specify a sequence\n");
  printf("  -v --version prints the svn revision number and exits\n");
  printf("  -ns do NOT print to the screen\n");
  printf("  -nk do NOT not check the keyboard\n");
  printf("  -bw use busy wait\n");
  printf("\n");
}


/*****************************************************************/

int main(int argc, char *argv[])
{
  //printf("%s:%d\n", __FILE__, __LINE__);
  if (SIG_ERR == signal(SIGHUP, react_signal_handler))
  {
    perror("Error setting signal SIGHUP");
    exit(1);
  }
  signal(SIGQUIT, my_sighandler);
  signal(SIGTERM, my_sighandler);
  signal(SIGFPE, my_sighandler);
  signal(SIGILL, my_sighandler);
  signal(SIGSEGV, my_sighandler);
  signal(SIGINT, my_sighandler);
  signal(SIGINT, my_sighandler);
  signal(SIGIO ,  SIG_IGN);

  //printf("%s:%d\n", __FILE__, __LINE__);
  bool execute_script = true;
  bool check_keyboard = true;
  bool print_to_screen = true;
  bool busy_wait = false;
  const char *home_dir = "./";
  int current_arg;
  for (current_arg=1; current_arg < argc; current_arg++)
  {
    if (0 == strcmp(argv[current_arg], "-x"))
    {
      // Don't execute a script, just control.
      execute_script = false;
    }
    else if (0 == strcmp(argv[current_arg], "-nk"))
    {
      // Don't check the keyboard
      check_keyboard = false;
    }
    else if (0 == strcmp(argv[current_arg], "-ns"))
    {
      // Don't print to the screen
      print_to_screen = false;
    }
    else if (0 == strcmp(argv[current_arg], "-v") || (0 == strcmp(argv[current_arg], "--version")))
    {
      printf( "react2, %s\n", 
            #include "revision_number"
            );
      exit(0);
    }
    else if (0 == strcmp(argv[current_arg], "-bw"))
    {
      // Use busy wait, only for very fast sampling, but not perfect.
      // You should not use this if you want to run ANY other processes.
      busy_wait = true;
    }
    else if (0 == strcmp(argv[current_arg], "-d"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        home_dir = argv[current_arg];
        chdir(home_dir);
        home_dir = "./";
      }
      else
      {
        print_help();
        printf("For -d option, you MUST specify the directory, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "--help"))
    {
      print_help();
      exit(0);
    }
    else if (argv[current_arg][0] == '-')
    {
      print_help();
      exit(1);
    }
    else
    {
      break;
    }
  }


  //printf("%s:%d\n", __FILE__, __LINE__);
  char secname[50];
  secname[0] = '\0';
  char secpath[200];
  safe_strcpy(secpath, home_dir, sizeof(secpath));
  safe_strcat(secpath, "/corridas/", sizeof(secpath));
  if (execute_script)
  {
    if (current_arg >= argc)
    {
      // If the -x option was not used, then prompt for the script if
      // it was not specified on the command line.
      printf("Indique la secuencia por correr: ");
      char temp[50];
      fgets(temp, sizeof(temp), stdin);
      ltrim(temp);
      rtrim(temp);
      safe_strcpy(secname, temp, sizeof(secname));
    }
    else
    {
      safe_strcpy(secname, argv[current_arg], sizeof(secname));
    }
    char sectmp[200];
    system("pwd");
    snprintf(sectmp, sizeof(sectmp), "%s/%s", secpath, secname);
    if (0 != access(sectmp, R_OK))
    {
      print_help();
      printf("Can not open the sequence file: %s\n", secname);
      exit(1);
    }
  }

  //printf("%s:%d\n", __FILE__, __LINE__);
  if (execute_script)
  {
    conioinit();
  }
  //printf("%s:%d\n", __FILE__, __LINE__);
  atexit(exit_clean_up);

  
  char config_file[100];
  safe_strcpy(config_file, home_dir, sizeof(config_file));
  safe_strcat(config_file, "/dbfiles/config.dat", sizeof(config_file));
  ap_config.read_file(config_file);
  logfile = new logfile_t(false, print_to_screen);
  logfile->set_startup_log_on();
  time_t tnow = time(NULL);
  logfile->vprint("%s", ctime(&tnow));
  logfile->vprint("React PID: %d\n", getpid());

  double sample_rate = ap_config.get_double("SampleRate", 1.0);
  if (sample_rate < 0.01)  
  {
    logfile->vprint("SampleRate out of range: %lf\n", sample_rate);
    sample_rate =  0.1;
  }
  logfile->vprint("SampleRate set to: %lf\n", sample_rate);
  long usecs_per_sample =  (long) (1000000.0 / sample_rate);
  reactdb = new react_t;
  db = reactdb;
  reactdb->keyboard_on(check_keyboard);
  reactdb->set_home_dir(home_dir);
  reactdb->set_sample_rate(sample_rate);
  reactdb->read_all_points(home_dir);
  reactdb->init_sms();


  int ntests = 0;
  if (execute_script)
  {
    system("pwd");
    reactdb->read_secuencia(secname, secpath);
    ntests = num_tests(secname);
    logfile->vprint("Foreground script: %s\n", secname);
  }

  reactdb->read_background_sequences(home_dir, secpath);
  

  reactdb->init_driver();
  reactdb->verify_drivers();
  //reactdb->print_all_points();

  printf("Num tests = %d\n", ntests);
  edit_files("inicial", ntests);


  tnow = time(NULL);

  logfile->vprint("%s", ctime(&tnow));

  timeaccum_t::enable(true);
  timejitter_t::enable(true);

  timeaccum_t ta;
  timejitter_t tj((double)usecs_per_sample / 1000000.0, 0.4);

  utimer_t utimer;
  utimer.set_busy_wait(busy_wait);
  struct timespec ts; 
  ts.tv_sec = usecs_per_sample / 1000000;
  ts.tv_nsec = (usecs_per_sample % 1000000) * 1000;
  utimer.set_interval(ts);
  utimer.set_start_time();
  bool done = false;

  if (not execute_script)
  {
    logfile->vprint("No foreground script executing, use \"kill -1 %d\" or <CTRL-C> to stop\n", getpid());
  }

  logfile->set_startup_log_off();
  system("pwd");
  for (int i=0; !done; i++)
  {
    ta.start();
    //printf("Waiting for the next interval . . . \n");
    utimer.wait_next();
    //printf("DONE, next upate . . . \n");
    tj.start_of_interval();
    done = reactdb->update((double) i / (double) sample_rate, execute_script);
    if (signal_recieved)
    {
      done = true;
    }
    ta.stop();
  }
  extern timeaccum_t ta1, ta2, ta4, taa[20];
  if (timeaccum_t::is_enabled())
  {
    printf("ta, n = %ld, avg = %lf, total = %lf\n", ta.get_count(), ta.get_average(), ta.get_total());
    printf("ta1, n = %ld, avg = %lf, total = %lf\n", ta1.get_count(), ta1.get_average(), ta1.get_total());
    printf("ta2, n = %ld, avg = %lf, total = %lf\n", ta2.get_count(), ta2.get_average(), ta2.get_total());
    printf("ta3, n = %ld, avg = %lf, total = %lf\n", ta4.get_count(), ta4.get_average(), ta4.get_total());
    for (int i=0; i < 13; i++)
    {
      taa[i].print(i);
    }
  }

  if (timejitter_t::is_enabled()) tj.print_results();

  if (!signal_recieved)
  {
    edit_files("final", ntests);
  }
  logfile->write_final_logs(ntests);
}

/**************************************************************************/

