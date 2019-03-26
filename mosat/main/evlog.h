
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
  void reopen_files(void);
  void check_day(void);
  void only_open_files(void);
  long open_files(bool find_seq);
  void file_name(const char *post, char *buf, int sz);
  int get_day(void);
public:
  evlog_t(char *the_dir);
  void print_event(long the_id, const char *str, time_t mytime);
  void print_log(long the_id, const char *tp, time_t start_time, 
                time_t end_time, int n_counts, int start_cnt, int end_cnt);
  long open_files(void);
  void printf(const char *fmt, ...);
};

#endif

