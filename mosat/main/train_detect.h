#ifndef __TRAIN_DETECT_INC__
#define __TRAIN_DETECT_INC__

class event_log_base_t
{
public:
  virtual void event_start(long id, time_t the_time, long counts) = 0;
  virtual void event_continue(long id, time_t the_time, long counts) = 0;
  virtual void event_end(long id, 
          time_t the_time, time_t start_time, time_t end_time,
          long total_counts, long start_count, long end_count) = 0;
  virtual void event_disconnect(long id, time_t the_time) = 0;
  virtual void event_reconnect(long id, time_t the_time) = 0;
};

class train_detector_t
{
private:
  bool detect;
  int detect_time;
  int last_time;
  int start_count;
  int last_count;
  int count_dif;
  int total_counts;
  long my_id;
  int disconnect_time;
  bool disconnect_detect;
  bool disconnected;
  event_log_base_t *evlog;

public:
  train_detector_t(void);
  void set_id(long an_id);
  void set_initial_count(int count);
  void set_evlog(event_log_base_t *evl);
  void update(int count, bool connect);
  static void cev_from_net(crossing_event_data_t *ev, void *buf, int buf_size);
  static void cev_to_net(crossing_event_data_t *ev, void *buf, int buf_size);
};

#define EVDT_WAITING (0)
#define EVDT_PENDING (1)
#define EVDT_WAIT_ALL (2)

#define EVDT_ARRIVAL (0) 
#define EVDT_DEPARTURE (1)

class event_detector_t
{
private:
  int state;
  time_t start_time;
  time_t saved_start_time; 
  time_t last_ad_change; 
  time_t start_confirm_time;
  time_t now;
  bool waiting_confirm;
  int confirm_now;
  int departure_last;
  int arrival_last;
  int confirm_last;
  int departure_change;
  int arrival_change;
  int confirm_change;
  int departure_first;
  int arrival_first;
  int confirm_first;
  int departure_dif;
  int arrival_dif;
  int confirm_dif;
  int departure_total;
  int arrival_total;
  int confirm_total;
  long departure_id;
  long arrival_id;
  long confirm_id;
  int event_type;
  int saved_event_type; 
  event_log_base_t *evlog;
  void send_event(time_t the_time, long the_event_type);
  void check_confirm(void);
  void waiting(void);
  void pending(void);
  void wait_all(void);
public:
  event_detector_t(void);
  void set_ids(long id1, long id2, long id3);
  void set_initial_count(int c1, int c2, int c3);
  void set_evlog(event_log_base_t *evl);
  void update(int c1, int c2, int c3);
};


#endif
