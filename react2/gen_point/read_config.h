

enum field_type_t {RT_FLOAT, RT_INT, RT_BOOL, RT_STRING, RT_SELECT, RT_ARRAY};

struct db_field_t
{
  field_type_t type;
  int length;
  char *prompt;
  char *name;
  char *extra_data;
  int n_select;
  char **select_prompts;
  char **select_names;

};


struct gen_names_t
{
  const char *table_name;
  const char *obj_type;
  const char *field_file_name;
  const char *data_file_name;
  db_field_t *dbfs[500];
  int nf;
};

void read_config_file(gen_names_t *gnames);
void print_field(db_field_t *dbf);
const char *field_spec(db_field_t *dbf, char *xspec, int sz);
db_field_t *create_field(int argc, char *argv[]);
const char *field_type_to_string(field_type_t f);


/************************************/


