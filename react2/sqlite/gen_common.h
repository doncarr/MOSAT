

typedef int (*write_one_t)(sqlite3 *sqdb, int argc, char *argv[]);

int insert_from_dat_file(sqlite3 *sqdb,
                 const char *fname, write_one_t write_fn);


