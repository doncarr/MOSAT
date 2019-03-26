


#define RT_MAX_RESTRICTIONS (200)

enum restriction_type_t
{
  RESTRICT_CURVE,
  RESTRICT_OTHER,
  RESTRICT_STATION,
  RESTRICT_CROSSING,
  RESTRICT_CONSTRUCTION,
  RESTRICT_SECTION,
  RESTRICT_MAINTENANCE,
  RESTRICT_MOD_NEXT,
  RESTRICT_ACCEL_DECEL_CONTROL
};


struct restriction_def_t
{
  restriction_type_t type;
  char description[40];
  double start;
  double end;
  double max_speed;
  double normal_stop_point;
};

class restriction_reader_t
{
private:
  int n_restrictions;
  restriction_def_t restriction[RT_MAX_RESTRICTIONS];
public:
  restriction_reader_t(void);
  int get_n_restrictions(void) {return n_restrictions;};
  const restriction_def_t *get_restriction(int n) {return  &restriction[n];}; 
  int read_restrictions(const char *fname);
  const char *restriction_string(restriction_type_t type);
  void print_restriction(const restriction_def_t *res);
};

