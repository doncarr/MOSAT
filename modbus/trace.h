
#ifndef __TRACE_INC__
#define __TRACE_INC__

#include <stdio.h>
//#include "common.h"

class debug_trace_t
{
private:
  int current_level;
  long file_size;
  int current_file;
  FILE *fp;
public:
  debug_trace_t(void);
  ~debug_trace_t(void);
  int dprintf(int level, const char *fmt, ... );
#ifdef __GNUC__
          __attribute__ (( format( printf, 3, 4 ) ));
#endif
  void dperror(int a_level, const char *prefix);
  void print_buf(int level, const char *prefix, uint8 *buf, int n);
  void set_level(int level) {current_level = level;};
  int get_level(void) {return current_level;};
  void enable_file_output();
  void stop_file_output();
  /****
  void init_modem(void);
  void hang_up_modem(void);
  void get_modem_command(MODSerial *cp);
  void print_to_modem(char *str);
  GFI8250 *modem_sio;
  GFSerial *modem_cp;
  GFDataFormat *data_format;
  GFLineStatus *line_stat;
  GFModem *hayes;
  int carrier;
  ***/
};

extern debug_trace_t react_trace;


#endif
