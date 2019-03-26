
#include <stdio.h>
#include <sqlite3.h>

#include "rtcommon.h"

struct callback_data_t
{
  bool first;
  int n;
};

/******************************************************************/

int my_callback(void *data, int n, char **vals, char **fields)
{
  callback_data_t *cbdata;
  cbdata = (callback_data_t *) data; // how to pass data to callback.
  cbdata->n++;
  /**
  if (cbdata->first) printf("First callback!!!\n");
  printf("Callback: %d, n = %d\n", cbdata->n, n);
  for (int i=0; i < n; i++)
  {
    printf("%d: field: '%s', value: '%s'\n", i, fields[i], vals[i]);
  }
  **/
  if (cbdata->first)
  {
    printf("    "); 
    for (int i=0; i < n; i++)
    {
      printf("%s ", fields[i]);
    }
    printf("\n");
  }
  printf("%d: ", cbdata->n); 
  for (int i=0; i < n; i++)
  {
    printf("'%s', ", vals[i]);
  }
  printf("\n");
  cbdata->first = false;
  return 0;
}

/******************************************************************/

int main(int argc, char *argv[])
{
  sqlite3 *sqdb;
  int retval;
  char *errmsg;
  char pattern[100];

  if (argc < 2)
  {
    printf("You must specify a script to read\n");
    return -1;
  }
  snprintf(pattern, sizeof(pattern), "%%");
  for (int i=1; i < argc; i++)
  {
    safe_strcat(pattern, argv[i], sizeof(pattern)); 
    safe_strcat(pattern, "%", sizeof(pattern)); 
  }


  retval = sqlite3_open_v2("react_def.db",  &sqdb,
     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (retval != SQLITE_OK)
  {
    printf("Can not open db, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    return 0;
  }

  char qs[300];

//select name from SQLITE_MASTER where type='table' order by name;

  snprintf(qs, sizeof(qs), 
  //"select instr from script where (tag='%s') order by n asc;", argv[1]);
  "select tag,n,instr from script where (tag like '%s') order by tag,n asc;", pattern);

  printf("qs = '%s'\n", qs);
  callback_data_t cbdata;
  cbdata.first = true;
  cbdata.n = 0;
  retval = sqlite3_exec(sqdb, qs, my_callback, &cbdata, &errmsg);
  if (retval != SQLITE_OK)
  {
    printf("Can not execute query, error = %d\n", retval);
    if (errmsg != NULL)
    {
      printf("errmsg: %s\n", errmsg);
      sqlite3_free(errmsg);
    }
    sqlite3_close(sqdb);
    return -1;
  }
  else if (cbdata.n == 0)
  {
    printf("Script does not exist: '%s'\n", argv[1]);
  }
  return 0;
}

