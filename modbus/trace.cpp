
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "trace.h"

debug_trace_t react_trace;

/************************************************************************/

debug_trace_t::debug_trace_t(void)
{
  current_level = 0;
  fp = NULL;
}

/************************************************************************/

debug_trace_t::~debug_trace_t(void)
{
}

/************************************************************************/

void debug_trace_t::dperror(int the_level, const char *extra)
{
  char buf[200];
  int save_errno = errno; 
  //snprintf(buf, sizeof(buf), "Unknown Error");
  //int err = 
  strerror_r(save_errno, buf, sizeof(buf)); 
  //if (err != 0) snprintf(buf, sizeof(buf), "Unknown Error");
  this->dprintf(the_level, "%s: %s\n", extra, buf);
}

/************************************************************************/

int debug_trace_t::dprintf(int level, const char *fmt, ... )
{
  va_list arg_ptr;
  unsigned long size;
  char buf[400];

  va_start(arg_ptr, fmt);

  size = vsnprintf(buf, sizeof(buf-1), fmt, arg_ptr);

  if (size >= sizeof(buf))
  {
    printf("Error, buffer too big for debug trace\n");
    return size;
  }

  if (level < current_level)
  {
    return size;
  }

  printf("%s", buf);
  //print_to_modem(buf);
  if (fp != NULL)
  {
    fprintf(fp, "%s", buf);
    fflush(fp);
    file_size += size;
    if (file_size > 100000)
    {
      fclose(fp);
      file_size = 0;
      switch (current_file)
      {
	case 0:
	  fp = fopen("/home/carr/debug1.out", "w");
	  if (fp == NULL)
	  {
	    perror("**** Can't open debug file");
	  }
	  current_file = 1;
	  break;
	case 1:
	  fp = fopen("/home/carr/debug0.out", "w");
	  if (fp == NULL)
	  {
	    perror("**** Can't open debug file");
	  }
	  current_file = 0;
	  break;
      }
    }
  }

  /*********
  if (level == 5)
  {
    printf("Level 5 error, exiting ......\n");
    exit(0);
  }
  **********/
  va_end(arg_ptr);
  return size;
}

/**********************************************************************/

void debug_trace_t::print_buf(int level,
	  const char *prefix, uint8 *bin_buf, int cnt)
{
  uint8 hex[2000];

  if (level < current_level)
  {
    return;
  }
  binary_to_hexascii(hex, bin_buf, cnt);
  dprintf(level, "%s %s\n", prefix, hex);

}

/**********************************************************************/

void debug_trace_t::enable_file_output()
{
  if (fp != NULL)
  {
    fclose(fp);
  }
  fp = fopen("c:\\sculptor\\debug0.out", "w");
  file_size = 0;
  current_file = 0;
  if (fp == NULL)
  {
    perror("**** Can't open debug file");
  }
}

/**********************************************************************/

void debug_trace_t::stop_file_output()
{
  fclose(fp);
  fp = NULL;
}

/**********************************************************************/

