

class time_table_t
{
private:
  char train_id[500][30];
  time_t times[500];
  bool matched[500];
  int next_match;
  int n_departures;
public:
  time_table_t(void);
  void read_day(void);
  const char *match_departure(time_t actual_departure_time);
  bool is_a_match(time_t scheduled_time, time_t actual_time);
  void next_day(void);
};


