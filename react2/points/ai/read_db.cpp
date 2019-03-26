#include <stdio.h>
#include <sqlite3.h>

struct callback_data_t
{
  bool first;
  int nf;
  int n;
};

/**********************/

int my_callback(void *data, int n, char **vals, char **fields)
{
  callback_data_t *cbdata;
  cbdata = (callback_data_t *) data; // how to pass data to callback.
  cbdata->n++;
  if (cbdata->first) printf("First callback!!!\n");
  printf("Callback: %d, n = %d\n", cbdata->n, n);
  if (n == cbdata->nf) printf("********** number of files MATCH!!\n");
  for (int i=0; i < n; i++)
  {
    printf("%d: field: '%s', value: '%s'\n", i, fields[i], vals[i]);
  }
  cbdata->first = false;
  return 0;
}

/**********************/

int main(int argc, char *argv[])
{
  sqlite3 *sqdb;
  sqlite3_stmt *stmt;
  const char *tail;
  int retval;
  char *errmsg;
  callback_data_t cbdata;

  retval = sqlite3_open_v2("react_def.db",  &sqdb,
     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (retval != SQLITE_OK)
  {
    printf("Can not open db, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    return 0;
  }


    /*--------*/

  printf("---- Reading database 'ai' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 21;
  retval = sqlite3_exec(sqdb, "select * from 'ai';",
             my_callback, &cbdata, &errmsg);
  if (retval != SQLITE_OK)
  {
    printf("Can not execute query, error = %d\n", retval);
    if (errmsg != NULL)
    {
      printf("errmsg: %s\n", errmsg);
      sqlite3_free(errmsg);
    }
    sqlite3_close(sqdb);
    return 0;
  }

}
