

int read_schedule(char *fname, int train_num[], time_t times[], int max, time_t today);
int read_actual(char *fname, time_t times[], int max, time_t today);
int n_read_actual(char *fname, time_t times[], int max, 
      time_t today, int exit_event);
