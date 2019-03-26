

#include "spd_algo.h"

struct sdef_t
{
  int n;
  char st1[13];
  double *dist;
  double *speed;
  double *low;
  double *high;
  double *very_high;
  double distance; 
  double time;
  double total_distance; 
  double total_time;
};

class profile_reader_t
{
private:
  int n_sections;
  sdef_t section_profile[RT_MAX_SECTIONS];

  int last_section;
  int my_index;
  int last_type;
  int type;
  bool all_profiles;
  int the_line;
public:
 profile_reader_t(void);
  /******
  double get_desired(double dist, int section, int index);
  double get_low(int section, int index);
  double get_high(int section, int index);
  double get__very_high(int section, int index);
  ********/
 int read_profiles(void);
 void set_line(int aLine) {the_line = aLine;};
 void set_all(bool all_profiles_on) {all_profiles = all_profiles_on;};
 void calc_desired(int a_section, double a_speed, double a_distance,
      current_speed_limits_t *limits, int *mode, bool *warn);
 double get_distance(int a_section) {return section_profile[a_section].distance;};
 double get_time(int a_section) {return section_profile[a_section].time;};
 double get_total_time(int a_section) {return section_profile[a_section].total_time;};
 double get_total_distance(int a_section) {return section_profile[a_section].total_distance;};
 char *get_station_name(int a_section) {return section_profile[a_section].st1;};
 int get_n_sections(void) {return n_sections;};
};

