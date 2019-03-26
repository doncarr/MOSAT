
#ifndef __PPC_LOGGER_INC__
#define __PPC_LOGGER_INC__

const long EV_WATCHDOG = 1001;
const long EV_NETDN = 1002;
const long EV_NETUP = 1003;
const long EV_NETFAIL = 1004;

struct event_log_header_t
{
  long seq_num; // Next sequence number to use.
  long index; // Where to write the next value.
  long total_valid; // How many are valid so far.
  time_t last_time;
};

struct crossing_event_data_t
{
  long seq_num;
  long id;
  long n_pulses;
  time_t start_time;
  time_t end_time;
};

/* This is a circular buffer of the most recent events, and will be mapped to a file */

#define EVMAXEVENTS (500)
struct ppcevlog_t
{
  event_log_header_t header;
  crossing_event_data_t events[EVMAXEVENTS];
};

class ppc_logger_t
{
private:
  int fd;
  FILE *fp;
  ppcevlog_t data;
  long event_location;
  time_t search_time;
  long search_seq_num;
  int search_index;
public:
  ppc_logger_t(char *fname);
  ~ppc_logger_t();
  long log_event(crossing_event_data_t *ev);
  int get_event(long seq_num, crossing_event_data_t *ev);
  void erase_log(void);
  //int find_event(time_t time, long seq_num);
  int find_event(long seq_num);
  //int find_oldest_event(time_t time);
  int find_oldest_event();
  //int first_event(time_t time, long seq_num, crossing_event_data_t *ev); 
  int first_event(long seq_num, crossing_event_data_t *ev); 
  int next_event(crossing_event_data_t *ev); 
};

inline const char *get_ev_string(long id)
{
  switch (id)
  {
    case EV_WATCHDOG: return "WD";
    case EV_NETDN: return "NETDN";
    case EV_NETUP: return "NETDN";
    case EV_NETFAIL: return "NETFAIL";
  }
  if ((id >= 50) && (id < 60)) return "SALIDA";
  if ((id >= 60) && (id < 70)) return "LLEGADA";
  if ((id >= 0) && (id < 100)) return "EVENT";
  if ((id >= 200) && (id < 300)) return "DISCON";
  if ((id >= 200) && (id < 300)) return "DISCON";
  if ((id >= 300) && (id < 400)) return "RECON";
  return "BADID";
};

void ppc_run_logger(char *config_file, char *error_log, 
       char *event_log, int idn);

#endif


