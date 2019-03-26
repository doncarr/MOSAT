
#include <stdio.h>
#include <time.h>

#include "rtcommon.h"

/* This is for logging startup information, and any subsequent configuration changes */
/* This should not be used for continuous logging of any kind */

/*
class rt_system_log_t
{
private:
  int n_errors_logged;
  int n_logged;
  char output_file_name[200];
  FILE *fp;
  bool log_to_stdout;
  bool log_to_file;
public:
  rt_system_log_t(void);
  void set_output_file_name(const char *a_file_name);
  void set_log_to_stdout(bool val) {log_to_stdout = val;};
  void set_log_to_file(bool val) {log_to_file = val;};
  void log_message(const char *message);
  void log_error(const char *message);
};

extern rt_system_log_t sys_log;
*/

/*****************************************************/

rt_system_log_t::rt_system_log_t(void)
{
  fp = NULL;
  output_file_name[0] = '\0';
  n_errors_logged = 0;
  n_logged = 0;
  log_to_stdout = true;
  log_to_file = false;
}

/*****************************************************/

void rt_system_log_t::set_output_file_name(const char *a_file_name)
{
  snprintf(output_file_name, sizeof(output_file_name), "%s", a_file_name);
  fp = fopen(output_file_name, "w");
  if (fp == NULL)
  {
    printf("In rt_system_log_t::set_file_name(%s):\n", a_file_name);
    perror(output_file_name);
    return;
  }
}

/*****************************************************/

void rt_system_log_t::log_message(const char *a_message)
{
  n_logged++;
  if (log_to_stdout)
  {
    printf("%s", a_message);
  }
  if ((log_to_file) && (fp != NULL))
  {
    fprintf(fp, "%s", a_message);
  }
}

/****************************************************/

void rt_system_log_t::log_error(const char *a_message)
{
  n_errors_logged++;
  this->log_message(a_message);
}

/****************************************************/
