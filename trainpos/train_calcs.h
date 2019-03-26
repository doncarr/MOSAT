

class train_calculator_t
{
private:
  unsigned n_trains;
  time_t time_now;
  train_data_t *td;
public:
  void set_time(time_t the_time_now){time_now = the_time_now;};
  int get_n_trains(void);
  int get_num(int train_index);
  const char *get_train_id(int train_index);
  time_t get_service_entry_time(int train_index);
  time_t get_scheduled_entry_time(int train_index);
  int get_line_location(int train_index);
  int get_section(int train_index);

  int get_seconds_late(int train_index);
  int seconds_in_section(int train_index);
  double get_fraction_of_section_traveled(int train_index);
  int get_seconds_to_next_train(int train_index);
  bool get_departed(int train_index);
  time_t get_arival_time(int train_index);
  time_t get_section_entry_time(int train_index);
  bool get_unexpected(int train_index);
  bool get_switched_direction(int train_index);
};


