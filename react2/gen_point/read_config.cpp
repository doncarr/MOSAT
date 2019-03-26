#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "read_config.h"

/************************************/

const char *field_type_to_string(field_type_t f)
{
  switch(f)
  {
    case RT_FLOAT:
      return "RT_FLOAT";
    case RT_INT:
      return "RT_INT";
    case RT_BOOL:
      return "RT_BOOL";
    case RT_STRING:
      return "RT_STRING";
    case RT_SELECT:
      return "RT_SELECT";
    case RT_ARRAY:
      return "RT_ARRAY";
  }
  return "";
}

/**********************************************************************/

db_field_t *create_field(int argc, char *argv[])
{
  db_field_t *f = new(db_field_t);
  if (0 == strcasecmp(argv[0], "string"))
  {
    f->type = RT_STRING;
    f->prompt = strdup(argv[1]);
    f->name = strdup(argv[2]);
    f->length = atoi(argv[3]);
  }
  else if (0 == strcasecmp(argv[0], "double"))
  {
    f->type = RT_FLOAT;
    f->prompt = strdup(argv[1]);
    f->name = strdup(argv[2]);
    f->length = 0;
  }
  else if (0 == strcasecmp(argv[0], "int"))
  {
    f->type = RT_INT;
    f->prompt = strdup(argv[1]);
    f->name = strdup(argv[2]);
    f->length = 0;
  }
  else if (0 == strcasecmp(argv[0], "bool"))
  {
    f->type = RT_BOOL;
    f->prompt = strdup(argv[1]);
    f->name = strdup(argv[2]);
    f->length = 0;
  }
  else if (0 == strncasecmp(argv[0], "array", 5))
  {
    f->type = RT_ARRAY;
    f->prompt = strdup(argv[1]);
    f->name = strdup(argv[2]);
    f->extra_data = strdup(argv[3]); // Definition of array in JSON format.
    f->length = 1;
  }
  else if (0 == strcasecmp(argv[0], "dropdown"))
  {
    if (argc != 5)
    {
      printf("Wrong number of args for dropdown\n");
      return NULL;
    }
    delim_separator_t ds_label(100, 20, ',');
    delim_separator_t ds_value(100, 20, ',');
    char **label_argv;
    int label_argc;
    char **value_argv;
    int value_argc;

    f->type = RT_SELECT;
    f->prompt = strdup(argv[1]);
    f->name = strdup(argv[2]);
    label_argv = ds_label.separate(&label_argc, argv[3]);
    value_argv = ds_value.separate(&value_argc, argv[4]);
    f->n_select = label_argc;
    f->select_prompts = new char*[label_argc];
    f->select_names = new char*[value_argc];
    int lmax = 0;
    for (int i=0; i < label_argc; i++)
    {
      f->select_prompts[i] = strdup(label_argv[i]);
      f->select_names[i] = strdup(value_argv[i]);
      int len = strlen(f->select_names[i]);
      if (len > lmax) lmax = len;
    }
    f->length = lmax;
  }
  else if (0 == strcasecmp(argv[0], "separator"))
  {
    return NULL;
  }
  else if (0 == strcasecmp(argv[0], "table"))
  {
    return NULL;
  }
  else
  {
    printf("Invalid type: %s\n", argv[0]);
    return NULL;
  }
  return(f);
}

/**********************************************************************/

const char *field_spec(db_field_t *dbf, char *xspec, int sz)
{
  switch(dbf->type)
  {
    case RT_FLOAT:
      snprintf(xspec, sz, "%s FLOAT", dbf->name);
      break;
    case RT_INT:
      snprintf(xspec, sz, "%s INT", dbf->name);
      break;
    case RT_BOOL:
      snprintf(xspec, sz, "%s BOOLEAN", dbf->name);
      break;
    case RT_STRING:
      snprintf(xspec, sz, "%s VARCHAR(%d)", dbf->name, dbf->length);
      break;
    case RT_SELECT:
      snprintf(xspec, sz, "%s VARCHAR(%d)", dbf->name, dbf->length);
      break;
    case RT_ARRAY:
      snprintf(xspec, sz, "%s VARCHAR(1)", dbf->name);
      break;
  }
  return xspec;
}

/**********************************************************************/


void print_field(db_field_t *dbf)
{
  printf("%s|%s|%s|%d|\n",
            field_type_to_string(dbf->type),
            dbf->prompt, dbf->name, dbf->length);
}



/**********************************************************************/

void read_config_file(gen_names_t *gnames)
{
  char **argv;
  int argc;
  int line_num;
  char obj_type_name[100];

  gnames->nf = 0;

  delim_file_t df(300, 20, '|', '#');
  df.print_lines(true);
  
  gnames->table_name = "table_name";
  gnames->obj_type = "obj_type_t";

  for (argv = df.first(gnames->field_file_name, &argc, &line_num);
         (argv != NULL); 
            argv = df.next(&argc, &line_num))
  {
    if (argc < 2)
    {
      printf("Wrong number of args, line %d\n", line_num);
      continue;
    }
    printf("%d: ", line_num);
    for (int i=0; i < argc; i++)
    {
      printf("%s ", argv[i]);
    }  
    printf("\n");
    if (0 == strcasecmp(argv[0], "table"))
    {
      gnames->table_name = strdup(argv[2]);
      if (argc > 3)
      {
      gnames->obj_type = strdup(argv[3]);
      }
      else
      {
        snprintf(obj_type_name, sizeof(obj_type_name), "%s_t", argv[2]);
        gnames->obj_type = strdup(obj_type_name);
      }
    }
    db_field_t *dbf; 
    dbf = create_field(argc, argv);
    if (dbf != NULL)
    {
      gnames->dbfs[gnames->nf] = dbf;
      gnames->nf++;
    }
    else
    {
      printf("Can't create field:");
      for (int i=0; i < argc; i++) printf(" '%s'", argv[i]);
      printf("\n");
    }
  }
  printf("Done parsing file, %d fields found\n", gnames->nf);
}

/**********************************************************************/
