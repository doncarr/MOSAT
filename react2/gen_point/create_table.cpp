
#include <stdio.h>
#include <sqlite3.h>

struct callback_data_t
{
  bool first;
  int n;
};

struct dbdata_t
{
  const char *str;
  int num;
};

dbdata_t ddata[] =
{
  {"Hello", 10},
  {"Goodbye", 20},
  {"who?", 30},
  {"what?", 40},
  {"where?", 45},
  {"when?", 50},
  {"why?", 55},
  {"This!", 60},
  {"That!", 65},
  {"These!", 70},
  {"Those!", 75},
  {"Apples", 80},
  {"Oranges", 85},
  {"Bananas", 90},
  {"Peaches", 95},
  {"Mangos", 99},
  {"Grapes", 100},
  {"Rasberrys", 101},
  {"Blueberrys", 102},
  {"Pinapples", 103},
  {"Strawberrys", 104}
};

int my_callback(void *data, int n, char **vals, char **fields)
{
  callback_data_t *cbdata;
  cbdata = (callback_data_t *) data; // how to pass data to callback.
  cbdata->n++;
  if (cbdata->first) printf("First callback!!!\n");
  printf("Callback: %d, n = %d\n", cbdata->n, n);
  for (int i=0; i < n; i++)
  {
    printf("%d: field: '%s', value: '%s'\n", i, fields[i], vals[i]);
  }
  cbdata->first = false;
  return 0;
}


int main(int argc, char *argv[])
{
  sqlite3 *sqdb;
  sqlite3_stmt *stmt;
  const char *tail;
  int retval;

  retval = sqlite3_open_v2("mydb4",  &sqdb, 
     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL); 
  if (retval != SQLITE_OK)
  {
    printf("Can not open db, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    return 0;
  }

  retval = sqlite3_prepare_v2(sqdb, 
        "create table tbl1(this_string varchar(10), that_number smallint);",
        100, &stmt, &tail);
  if (retval != SQLITE_OK)
  {
    printf("Can not prepare statement, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    sqlite3_close(sqdb);
    return 0;
  }
  printf("Tail: %s\n", tail);

  retval = sqlite3_step(stmt);
  if (retval != SQLITE_DONE)
  {
    printf("Can not execute statement, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    sqlite3_close(sqdb);
    return 0;
  }

  retval = sqlite3_finalize(stmt);
  if (retval != SQLITE_OK)
  {
    printf("Can not finalize statement, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    sqlite3_close(sqdb);
    return 0;
  }

  for (unsigned i = 0; i < (sizeof(ddata)/sizeof(ddata[0])); i++)
  {
    char str[200];
    snprintf(str, sizeof(str), "insert into 'tbl1' values('%s',%d);",
         ddata[i].str, ddata[i].num); 
    printf("sql: %s\n", str);

    retval = sqlite3_prepare_v2(sqdb, str, 100, &stmt, &tail);
    if (retval != SQLITE_OK)
    {
      printf("Can not prepare statement, error = %d\n", retval);
      printf("Err = %s\n", sqlite3_errmsg(sqdb));
      sqlite3_close(sqdb);
      return 0;
    }
    printf("Tail: %s\n", tail);

    retval = sqlite3_step(stmt);
    if (retval != SQLITE_DONE)
    {
      printf("Can not execute statement, error = %d\n", retval);
      printf("Err = %s\n", sqlite3_errmsg(sqdb));
      sqlite3_close(sqdb);
      return 0;
    }

    retval = sqlite3_finalize(stmt);
    if (retval != SQLITE_OK)
    {
      printf("Can not finalize statement, error = %d\n", retval);
      printf("Err = %s\n", sqlite3_errmsg(sqdb));
      sqlite3_close(sqdb);
      return 0;
    }
  }

  char *errmsg;

  /* Ok, this is the easy to do it!, why mess with prepare, step, finalize? */
  retval = sqlite3_exec(sqdb, 
        "create table tbl2(a_string varchar(10), a_number smallint);",
             NULL, NULL, &errmsg);
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
  for (unsigned i = 0; i < (sizeof(ddata)/sizeof(ddata[0])); i++)
  {
    char str[200];
    snprintf(str, sizeof(str), "insert into 'tbl2' values('%s',%d);",
         ddata[i].str, ddata[i].num);

    retval = sqlite3_exec(sqdb, str, NULL, NULL, &errmsg);
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


  /* Ok, print the first table */
  callback_data_t cbdata;
  cbdata.first = false;
  cbdata.n = 0;
  retval = sqlite3_exec(sqdb, "select * from 'tbl1';", 
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

  /* Ok, now print the second table */
  cbdata.first = false;
  cbdata.n = 0;
  retval = sqlite3_exec(sqdb, "select * from 'tbl2';",
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


  sqlite3_close(sqdb);
  return 0;
}


