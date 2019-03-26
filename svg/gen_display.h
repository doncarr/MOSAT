

enum rtsvg_param_t {rtsvg_X, rtsvg_Y, rtsvg_WIDTH, rtsvg_HEIGHT, rtsvg_TEXT, rtsvt_INT, rtsvg_FLOAT, rtsvg_BOOL};

char *check_params(rtsvg_param_t params[]);

struct plugin_data_t
{
  FILE *svg_fp;
  FILE *svg_top_of_file_fp;
  FILE *js_fp;
  bool popup_on;
  const char *file_name;
  int line_number;
  bool silent;
};

class doc_object_base_t
{
protected:
  FILE *fp;
  int param_number;
  int note_number;
  int example_number;
public:
  void set_fp(FILE *the_fp){this->fp = the_fp;};
  virtual void header(void)=0;
  virtual void start(const char *name, const char *short_desc)=0;
  virtual void param(const char *short_desc)=0;
  virtual void example(const char *example)=0;
  virtual void notes(const char *notes)=0;
  virtual void end(void)=0;
  virtual void footer(void)=0;
};

class doc_html_object_t : public doc_object_base_t
{
public:
  void header(void);
  void set_fp(FILE *the_fp);
  void start(const char *name, const char *short_desc);
  void param(const char *short_desc);
  void example(const char *example);
  void notes(const char *notes);
  void end(void);
  void footer(void);
};

class doc_text_object_t : public doc_object_base_t
{
public:
  void header(void);
  void set_fp(FILE *the_fp);
  void start(const char *name, const char *short_desc);
  void param(const char *short_desc);
  void example(const char *example);
  void notes(const char *notes);
  void end(void);
  void footer(void);
};

class gen_plugin_base_t
{
public:
  virtual const char *get_name(void)=0;
  virtual void generate_doc(doc_object_base_t *doc_base)=0;
  virtual void generate(plugin_data_t d, int argc, char **argv)=0;
  virtual rtsvg_param_t *param_types(int *n) {*n=0; return NULL;};
  virtual ~gen_plugin_base_t() {};
};

void add_js_library(const char *file_name);
void add_svg_library(const char *file_name);
void add_animation_object(const char *the_tag, const char *the_js_object);
void set_viewbox(double x1, double y1, double x2, double y2);

void find_a_place_nearby(double *x,double *y,
                         double screen_width, double screen_height,
                         double item_x, double item_y,
                         double item_width, double item_height,
                         double popup_width, double popup_height);

void find_a_place_nearby(double *x,double *y,
                         double item_x, double item_y,
                         double item_width, double item_height);



typedef gen_plugin_base_t * (*crear_objeto_t)(void);



