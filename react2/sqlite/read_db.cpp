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

  printf("---- Reading database 'remote_pid' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 24;
  retval = sqlite3_exec(sqdb, "select * from 'remote_pid';",
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

    /*--------*/

  printf("---- Reading database 'discrete_calc' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 7;
  retval = sqlite3_exec(sqdb, "select * from 'discrete_calc';",
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

    /*--------*/

  printf("---- Reading database 'pump' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 8;
  retval = sqlite3_exec(sqdb, "select * from 'pump';",
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

    /*--------*/

  printf("---- Reading database 'ac' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 7;
  retval = sqlite3_exec(sqdb, "select * from 'ac';",
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

    /*--------*/

  printf("---- Reading database 'level' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 6;
  retval = sqlite3_exec(sqdb, "select * from 'level';",
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

    /*--------*/

  printf("---- Reading database 'pid' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 19;
  retval = sqlite3_exec(sqdb, "select * from 'pid';",
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

    /*--------*/

  printf("---- Reading database 'ao' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 15;
  retval = sqlite3_exec(sqdb, "select * from 'ao';",
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

    /*--------*/

  printf("---- Reading database 'do' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 8;
  retval = sqlite3_exec(sqdb, "select * from 'do';",
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

    /*--------*/

  printf("---- Reading database 'web' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 5;
  retval = sqlite3_exec(sqdb, "select * from 'web';",
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

    /*--------*/

  printf("---- Reading database 'ai' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 23;
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

    /*--------*/

  printf("---- Reading database 'calc' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 16;
  retval = sqlite3_exec(sqdb, "select * from 'calc';",
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

    /*--------*/

  printf("---- Reading database 'discrete_value' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 5;
  retval = sqlite3_exec(sqdb, "select * from 'discrete_value';",
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

    /*--------*/

  printf("---- Reading database 'analog_value' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 7;
  retval = sqlite3_exec(sqdb, "select * from 'analog_value';",
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

    /*--------*/

  printf("---- Reading database 'valve' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 6;
  retval = sqlite3_exec(sqdb, "select * from 'valve';",
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

    /*--------*/

  printf("---- Reading database 'scan' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 5;
  retval = sqlite3_exec(sqdb, "select * from 'scan';",
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

    /*--------*/

  printf("---- Reading database 'pci' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 23;
  retval = sqlite3_exec(sqdb, "select * from 'pci';",
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

    /*--------*/

  printf("---- Reading database 'discrete_logger' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 5;
  retval = sqlite3_exec(sqdb, "select * from 'discrete_logger';",
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

    /*--------*/

  printf("---- Reading database 'data' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 5;
  retval = sqlite3_exec(sqdb, "select * from 'data';",
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

    /*--------*/

  printf("---- Reading database 'timer' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 7;
  retval = sqlite3_exec(sqdb, "select * from 'timer';",
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

    /*--------*/

  printf("---- Reading database 'di' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 10;
  retval = sqlite3_exec(sqdb, "select * from 'di';",
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

    /*--------*/

  printf("---- Reading database 'file_logger' -------------\n");
  cbdata.first = false;
  cbdata.n = 0;
  cbdata.nf = 11;
  retval = sqlite3_exec(sqdb, "select * from 'file_logger';",
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
