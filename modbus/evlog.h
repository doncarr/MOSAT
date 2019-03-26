
#ifndef __EVLOG_INC__
#define __EVLOG_INC__

//int get_day(void);
void evlog_print_event(long the_id, const char *str, time_t mytime);
void evlog_print_log(long the_id, const char *tp, time_t start_time, 
                time_t end_time, int n_counts, int start_cnt, int end_cnt);
long open_evlog_files(void);
void evlog_printf(const char *fmt, ...);

class evlog_t
{
private:
  char *local_dir;
  FILE *fp1;
  FILE *fp2;
  int theday;
  void reopen_files(time_t the_time);
  void check_day(time_t the_time);
  void only_open_files(time_t the_time);
  void file_name(time_t the_time, char *post, char *buf, int sz);
  int get_day(time_t the_time);
public:
  evlog_t(char *the_dir);
  void print_event(long the_id, const char *str, time_t mytime);
  void print_log(long the_id, const char *tp, time_t start_time, 
                time_t end_time, int n_counts, int start_cnt, int end_cnt);
  long open_files(time_t the_time, bool find_seq);
  long open_files(time_t the_time);
  void printf(const char *fmt, ...);
};

#endif

