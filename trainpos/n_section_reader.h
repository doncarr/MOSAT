
#define TP_MAX_SECTIONS (50)
#define TP_MAX_SENSORS (10)
#define TP_MAX_GLOBAL_SENSORS (200)

// Proposed structure for sensor information
struct sensor_info_t
{
  unsigned section;
  unsigned num_in_section;
  unsigned global_loc;
};

struct tsecdata_t
{
  int section_time;
  unsigned n_sensors;
  unsigned sensor_location[TP_MAX_SENSORS];
  int time_to_start;
  int dwell_time;
  char station[20];
};


class section_reader_t
{
private: 
  unsigned n_sections;
  unsigned n_global_sensors;
  tsecdata_t sections[TP_MAX_SECTIONS];
  sensor_info_t sensors[TP_MAX_GLOBAL_SENSORS];  
public:
  unsigned get_n_sections(void);
  unsigned get_n_sensors(unsigned section_number);
  int get_section_time(unsigned section_number);

  void read_section_file(void);

  // The following two functions must go away!!
  // The model must allow a variable numer of sensors in
  // each section, including one or zero sensors.
  unsigned get_departure_sensor_loc(unsigned section_number);
  unsigned get_arival_sensor_loc(unsigned section_number);

  int get_time_to_start(unsigned section_number);
  unsigned get_sensor_loc(unsigned section_number, unsigned sensor_number);
  const char *get_station_name(unsigned section_number);
  const tsecdata_t *get_section_data(unsigned section_number);
  // New functions
  unsigned get_sensor_loc(unsigned global_sensor_number);
  int get_dwell_time(unsigned section_number);
  const sensor_info_t *get_sensor_info(unsigned global_sensor_number);
  unsigned get_n_global_sensors(void);
};

extern section_reader_t sections;

