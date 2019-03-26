#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "rtcommon.h"
#include "ap_config.h"
#include "../logfile.h"

struct apconfig_cbdata_t
{
   ap_config_t *apconfig;
   int count;
};

int apconfig_callback(void *data, int n, char **vals, char **fields)
{
  apconfig_cbdata_t *cbdata =
    (apconfig_cbdata_t *) data;

  cbdata->count++;
  logfile->vprint("%d: ", cbdata->count);
  for (int i=0; i < n; i++)
  {
    logfile->vprint("'%s':'%s', ", fields[i], vals[i]);
  }
  cbdata->apconfig->insert_value(vals[0],vals[1]);
  logfile->vprint("\n");
  return 0;
}
void sqlite_apconfig_read(sqlite3 *sqdb, ap_config_t *apconfig)
{
  apconfig_cbdata_t cbdata;
  char *errmsg;
  int retval;

  cbdata.apconfig = apconfig;
  logfile->vprint("sqlite read apconfig\n");
  const char *query_string = "select * from 'config';"; 
  retval = sqlite3_exec(sqdb, query_string,
           apconfig_callback, &cbdata, &errmsg);
  if (retval != SQLITE_OK)
  {
    logfile->vprint("Can not execute query: %s, error = %d\n", 
                     query_string, retval);
    if (errmsg != NULL)
    {
      logfile->vprint("errmsg: %s\n", errmsg);
      sqlite3_free(errmsg);
    }
    //sqlite3_close(sqdb);
    return;
  }
  return;
}
