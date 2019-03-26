
#ifndef __SIM_READER_INC__
#define __SIM_READER_INC__

#include "spd_comm.h"

struct spd_sim_data_t
{
  int n;
  double speed[500];
  double dist[500];
};


class sim_reader_t
{
private:
  int n_sections;
  int the_line;
  spd_sim_data_t sim_data[RT_MAX_SECTIONS];
  int end_count; // Used to simulate doors opening;
  int start_count; // Used to simulate doors opening
public:
  sim_reader_t(void);
  int get_n_sections(void) {return n_sections;};
  double get_total_dist(int section);
  double get_total_time(int section) {return double(sim_data[section].n);};
  void set_line(int aLine) {the_line = aLine;};
  void get_sim_speed_dist(int section, int t, double *dist, double *speed, spd_discrete_t *des);
  void read_sim_data(void);
};

#endif
