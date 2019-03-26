


struct display_info_t // For reading the display info from a file
{
  char background[80];
  char square[80];
  char square_unexpected[80];
  char html_out[80];
  unsigned n_sections;
  int x1;
  int x2;
  int y1;
  int y2;
  int text_offset_y1;
  int text_offset_y2;
};


class display_reader_t
{
  unsigned n_displays;
  display_info_t *displays[20];
public:
  display_reader_t(void);
  unsigned get_n_displays(void) {return n_displays;};
  const display_info_t *get_display_data(unsigned n);
  void read_file(const char *fname);
};

class display_dist_t
{
/***
       x1                                      x2
       |                                       |
    y1 ------------------------------------------
       ------------------------------------------
       |                                       |
       |                                       |
    y2 ------------------------------------------
       ------------------------------------------
****/
private:
  int x1;
  int x2;
  int y1;
  int y2;
  int total_x_pixels;
  unsigned n_sections;
  unsigned n_squares;
  int return_start;
public:
  display_dist_t(int a_x1, int a_x2, int a_y1, int a_y2, int n_sections);
  void calc_xy(int section, double fraction, int *x, int *y, bool *coming_back);
};


class display_data_t
{
private:
  bool valid;
  FILE *fp;
  int  fd;
  display_dist_t *dd;
  const char *square;
  const char *square_unexpected;
  const char *background;
  const char *fname;
  int text_offset_y1;
  int text_offset_y2;
public:
  void set(const char *a_background, const char *a_square,
       const char *a_unexpected, const char *a_fname,
       int a_x1, int a_x2, int a_y1, int a_y2, int text_offset_y1, int text_offset_y2, int n_sections);
  void read(const char *base_name);
  void gen_html(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains);
};

static display_reader_t reader;

class gen_displays_t
{
private:
  FILE *fp;
  int fd;
  FILE *table_fp;
  int table_fd;
  FILE *perf_fp;
  int perf_fd;

  int n_sections;
  int n_displays;
   display_data_t *ddata;
public:
  void init(void);
  void gen_table(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains);
  void gen(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains);
  void gen_performance(time_t now, train_data_t *trains, train_calcs_t *calcs, int n_trains); 
};

