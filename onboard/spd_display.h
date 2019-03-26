
#include "onboard.h"

struct onboard_performance_t
{
  int last_segment_actual;
  int last_segment_desired;
  int total_actual;
  int total_desired;
};

int spd_beep(void);
int spd_getch(void);
int spd_endwin(void);
void spd_print_auto(bool b);
int spd_wait_key(const char *msg);
int spd_prompt_user(const char *msg1, const char *msg2);
void spd_redraw_segment(void);
void spd_init_segment(const char *st1, const char *st2);
void spd_init_screen();
void spd_redraw_all();
void spd_print_current(double desired, double actual, int type, bool warn, 
       double pct, double total_distance, double now, double total_time, spd_discrete_t *discretes);
void spd_show_loading(int time);
void spd_show_performance(const onboard_performance_t *perf);
void spd_create_image(const char *base_name, const char *gtitle, bool window);
int select_from_list(int n, const char *list[], const char *prompt);
