
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include <sqlite3.h>

#include "rtcommon.h"

int main(int argc, char *argv[])
{
  DIR *dir;
  struct dirent *dent;
  char script_name[100];
  char file_name[100];
  sqlite3 *sqdb;
  int retval;
  char *errmsg;

  if (argc < 2)
  {
    printf("You must specify a directory\n");
    return -1;
  }
  dir = opendir(argv[1]);

  if (dir == NULL)
  {
    perror(argv[1]);
    return -1;
  }

  retval = sqlite3_open_v2("react_def.db",  &sqdb,
     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (retval != SQLITE_OK)
  {
    printf("Can not open db, error = %d\n", retval);
    printf("Err = %s\n", sqlite3_errmsg(sqdb));
    return 0;
  }

  retval = sqlite3_exec(sqdb,
      "create table script(tag VARCHAR(25),n INTEGER,instr VARCHAR(50), PRIMARY KEY (tag,n));",
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

  
  dent = readdir(dir);
  while (dent != NULL)
  {
    int len = strlen(dent->d_name);
    if (dent->d_type == DT_REG) 
    {
      snprintf(script_name, sizeof(script_name), "%s", dent->d_name);
      snprintf(file_name, sizeof(file_name), "%s/%s", argv[1], dent->d_name);
      printf("%s\n", script_name); 
      printf("%s\n", file_name); 
      FILE *fp = fopen(file_name, "r");
      if (fp == NULL)
      {
        printf("Can't open %s\n", file_name);
        return -1;
      }
      char line[300];

      for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
      {
         rtrim(line);
         printf("%s\n", line);
         char qs[500];
         snprintf(qs, sizeof(qs), 
           "insert into \"script\" values('%s',%d,'%s');",
            script_name, i+1, line);
         printf("qs =%s\n", qs);
         retval = sqlite3_exec(sqdb, qs, NULL, NULL, &errmsg);
         if (retval != SQLITE_OK)
         {
           printf("Can not execute query, error = %d\n", retval);
           if (errmsg != NULL)
           {
             printf("errmsg: %s\n", errmsg);
             sqlite3_free(errmsg);
           }
           return -1;
         }
      }
    }
    dent = readdir(dir);
  }
}
