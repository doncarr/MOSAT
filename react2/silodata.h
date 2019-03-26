

#define SILO_DATA_TAG 0
#define SILO_DATA_COLUMN 1

struct tag_data_t
{
  char tag[50]; 
  char type[20];
  char value[10]; 
};

struct temp_column_data_t
{
  short column;
  short n_temps;
  short temps[32];  // temperature * 100
};

struct silodata_t
{
  short ctype;
  char key[100];
  time_t the_time; 

  union
  {
    tag_data_t td;
    temp_column_data_t cd;
  };
};

