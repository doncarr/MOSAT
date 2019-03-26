#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "rtcommon.h"
#include "arg.h"

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
  const char *base_name;
  const char *relative_name;
  db_field_t *dbfs[500];
  int nf;
};

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

const char *gen_value(db_field_t *dbf, int i, char *buf, int sz)
{
  const char *indent = "          ";
  switch(dbf->type)
  {
    case RT_FLOAT:
      snprintf(buf, sz, ",\n%sargv[%d]", indent, i);
      break;
    case RT_INT:
      snprintf(buf, sz, ",\n%sargv[%d]", indent, i);
      break;
    case RT_BOOL:
      snprintf(buf, sz, 
         ",\n%s((argv[%d][0] == 't') || (argv[%d][0] == 'T') || (argv[%d][0] == 'y') || (argv[%d][0] == 'Y') || (argv[%d][0] == '1')) ? \"1\" : \"0\"", 
         indent, i, i, i, i, i);
      break;
    case RT_STRING:
      snprintf(buf, sz, ",\n%sargv[%d]", indent, i);
      break;
    case RT_SELECT:
      snprintf(buf, sz, ",\n%sargv[%d]", indent, i);
      break;
    case RT_ARRAY:
      snprintf(buf, sz, ",\n%sargv[%d]", indent, i);
      break;
  }
  return buf;
}

/**********************************************************************/

const char *gen_format(db_field_t *dbf, char *buf, int sz)
{
  switch(dbf->type)
  {
    case RT_FLOAT:
      snprintf(buf, sz, "%%s");
      break;
    case RT_INT:
      snprintf(buf, sz, "%%s");
      break;
    case RT_BOOL:
      snprintf(buf, sz, "%%s");
      break;
    case RT_STRING:
      snprintf(buf, sz, "'%%s'");
      break;
    case RT_SELECT:
      snprintf(buf, sz, "'%%s'");
      break;
    case RT_ARRAY:
      snprintf(buf, sz, "'%%s'");
      break;
  }
  return buf;
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

void gen_read_dat(FILE *fp_out, gen_names_t *gnames)
{
  fprintf(fp_out, "\n\n/***************************/\n\n");
  fprintf(fp_out, "int read_dat_file(sqlite3 *sqdb, const char *fname)\n");
  fprintf(fp_out, "{\n");
  fprintf(fp_out, "  FILE *fp = fopen(fname, \"r\");\n");
  fprintf(fp_out, "  if (fp == NULL)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    perror(fname);\n");
  fprintf(fp_out, "    return -1;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  char line[300];\n");

  fprintf(fp_out, "  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    int argc;\n");
  fprintf(fp_out, "    char *argv[25];\n");
  fprintf(fp_out, "    char tmp[300];\n");
  //fprintf(fp_out, "    safe_strcpy(tmp, line, sizeof(tmp));\n");
  fprintf(fp_out, "    snprintf(tmp, sizeof(tmp), \"%%s\", line);\n");
  fprintf(fp_out, "    argc = get_delim_args(tmp, argv, '|', 25);\n");
  fprintf(fp_out, "    if (argc == 0)\n");
  fprintf(fp_out, "    {\n");
  fprintf(fp_out, "      continue;\n");
  fprintf(fp_out, "    }\n");
  fprintf(fp_out, "    else if (argv[0][0] == '#')\n");
  fprintf(fp_out, "    {\n");
  fprintf(fp_out, "      continue;\n");
  fprintf(fp_out, "    }\n");
  fprintf(fp_out, "    printf(\"%%s\\n\", line);\n");
  fprintf(fp_out, "    write_one_%s(sqdb, argc, argv);\n", gnames->table_name);
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  return 0;\n");
  fprintf(fp_out, "}\n");
}

/**********************************************************************/

void  gen_header(FILE *fp_out, const char *exe_name)
{
  fprintf(fp_out, "/***************************\n");
  fprintf(fp_out, "This is an auto-generated file, do NOT edit!!\n");
  fprintf(fp_out, "   generated by %s\n", exe_name);
  fprintf(fp_out, "***************************/\n");
  fprintf(fp_out, "#include <stdio.h>\n");
  fprintf(fp_out, "#include <sqlite3.h>\n");
  fprintf(fp_out, "#include \"rtcommon.h\"\n");
  fprintf(fp_out, "#include \"arg.h\"\n");
}

/**********************************************************************/

void write_main_start(FILE *fp_out)
{
  fprintf(fp_out, "#include \"gen_common.h\"\n");

  fprintf(fp_out, "int main(int argc, char *argv[])\n");
  fprintf(fp_out, "{\n");
  fprintf(fp_out, "  sqlite3 *sqdb;\n");
  fprintf(fp_out, "  int retval;\n");
  fprintf(fp_out, "  char *errmsg;\n");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "  printf(\"Creating 'react_def.db'\\n\");\n");
  fprintf(fp_out, "  retval = sqlite3_open_v2(\"react_def.db\",  &sqdb,\n");
  fprintf(fp_out, "     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);\n");
  fprintf(fp_out, "  if (retval != SQLITE_OK)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    printf(\"Can not open db, error = %%d\\n\", retval);\n");
  fprintf(fp_out, "    printf(\"Err = %%s\\n\", sqlite3_errmsg(sqdb));\n");
  fprintf(fp_out, "    return 0;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  else\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    printf(\"SUCCESS\\n\");\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  \n");
}

/**********************************************************************/

void write_main_end(FILE *fp_out)
{
  fprintf(fp_out, "  sqlite3_close(sqdb);\n");
  fprintf(fp_out, "}\n");
}

/**********************************************************************/

void xprocess_file(FILE *fp_out, gen_names_t *gnames)
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
      continue;
    }
    else if (0 == strcasecmp(argv[0], "separator"))
    {
      // separator is only for user interface 
      continue;
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

void process_file(FILE *fp_out, gen_names_t *gnames)
{
  char **argv;
  int argc;
  int line_num;

  gnames->nf = 0;

  delim_file_t df(300, 20, '|', '#');
  df.print_lines(true);


  for (argv = df.first(gnames->field_file_name, &argc, &line_num);
         (argv != NULL); 
            argv = df.next(&argc, &line_num))
  {
    if (argc < 2)
    {
      printf("Wrong number of args, line %d\n", line_num);
      continue;
    }
    printf("%d: %s", line_num, argv[2]);
    for (int i=1; i < argc; i++)
    {
      printf(", %s", argv[i]);
    }  
    printf("\n");
    if (0 == strcasecmp(argv[0], "table"))
    {
      gnames->table_name = strdup(argv[2]);
    }
    db_field_t *dbf; 
    dbf = create_field(argc, argv);
    if (dbf != NULL)
    {
      gnames->dbfs[gnames->nf] = dbf;
      gnames->nf++;
    }
  }
  printf("Done parsing file, %d fields found\n", gnames->nf);

  char qs[5000];
  char fspec[100];
  snprintf(qs, sizeof(qs), "create table %s(", gnames->table_name);
 
// "create table tbl2(a_string varchar(10), a_number smallint);",
  int first = true;
  for (int i=0; i < gnames->nf; i++)
  {
    if (!first) safe_strcat(qs, ", ", sizeof(qs));
    safe_strcat(qs, field_spec(gnames->dbfs[i], fspec, sizeof(fspec)), sizeof(qs));
    first = false;
  }
  safe_strcat(qs, ", PRIMARY KEY (tag)", sizeof(qs)); 
  safe_strcat(qs, ");", sizeof(qs));
  printf("%s\n", qs);
  printf("%s\n", gnames->table_name);

  fprintf(fp_out, "\n\n/***************************/\n\n");
  fprintf(fp_out, "int write_one_%s(sqlite3 *sqdb, int argc, char *argv[]);\n",
            gnames->table_name);
  fprintf(fp_out, "int read_dat_file(sqlite3 *sqdb, const char *fname);\n");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "int main(int argc, char *argv[])\n");
  fprintf(fp_out, "{\n");
  fprintf(fp_out, "  sqlite3 *sqdb;\n");
  fprintf(fp_out, "  int retval;\n");
  fprintf(fp_out, "  char *errmsg;\n");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "  retval = sqlite3_open_v2(\"react_%s.sqlite\",  &sqdb,\n", 
         gnames->table_name);
  fprintf(fp_out, "     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);\n");
  fprintf(fp_out, "  if (retval != SQLITE_OK)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    printf(\"Can not open db, error = %%d\\n\", retval);\n");
  fprintf(fp_out, "    printf(\"Err = %%s\\n\", sqlite3_errmsg(sqdb));\n");
  fprintf(fp_out, "    return 0;\n"); 
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  \n");
  fprintf(fp_out, "  retval = sqlite3_exec(sqdb,\n");
  fprintf(fp_out, "      \"%s\",\n", qs);
  fprintf(fp_out, "             NULL, NULL, &errmsg);\n");
  fprintf(fp_out, "  if (retval != SQLITE_OK)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    printf(\"qs = %s\\n\");\n", qs);
  fprintf(fp_out, "    printf(\"Can not execute query, error = %%d\\n\", retval);\n");
  fprintf(fp_out, "    if (errmsg != NULL)\n");
  fprintf(fp_out, "    {\n");
  fprintf(fp_out, "      printf(\"errmsg: %%s\\n\", errmsg);\n");
  fprintf(fp_out, "      sqlite3_free(errmsg);\n");
  fprintf(fp_out, "    }\n");
  fprintf(fp_out, "    sqlite3_close(sqdb);\n");
  fprintf(fp_out, "    return 0;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  read_dat_file(sqdb, \"%s\");\n", gnames->data_file_name);
  fprintf(fp_out, "  sqlite3_close(sqdb);\n");
  fprintf(fp_out, "}\n");
}

/**********************************************************************/

void  gen_field_header(FILE *fp_out, const char *exe_name)
{
  fprintf(fp_out, "/***************************\n");
  fprintf(fp_out, "This is an auto-generated file, do NOT edit!!\n");
  fprintf(fp_out, "   generated by %s\n", exe_name);
  fprintf(fp_out, "***************************/\n");
  fprintf(fp_out, "#include <stdio.h>\n");
  fprintf(fp_out, "#include <stdlib.h>\n");
  fprintf(fp_out, "#include \"rtcommon.h\"\n");
  fprintf(fp_out, "#include \"arg.h\"\n");
  fprintf(fp_out, "#include \"db_point.h\"\n");
  fprintf(fp_out, "#include \"logfile.h\"\n");
}

/**********************************************************************/

void gen_create_one_field_to_obj(FILE *fp_out, db_field_t *dbf, int argn)
{
  switch (dbf->type)
  {
    case RT_FLOAT:
      fprintf(fp_out, "  objp->%s = atof(argv[%d]);\n", dbf->name, argn);
      break;
    case RT_INT:
      fprintf(fp_out, "  objp->%s = atoi(argv[%d]);\n", dbf->name, argn);
      break;
    case RT_BOOL:
      fprintf(fp_out, "  objp->%s = (argv[%d][0] == '1') ||\n", dbf->name, argn);
      fprintf(fp_out, "             (argv[%d][0] == 'T') ||\n", argn);
      fprintf(fp_out, "             (argv[%d][0] == 't');\n", argn);
      break;
    case RT_STRING:
      fprintf(fp_out, "  snprintf(objp->%s, sizeof(objp->%s), \"%%s\", argv[%d]);\n", dbf->name, dbf->name, argn);
      break;
    case RT_SELECT:
      fprintf(fp_out, "  snprintf(objp->%s, sizeof(objp->%s), \"%%s\", argv[%d]);\n", dbf->name, dbf->name, argn);
      break;
    case RT_ARRAY:
      break;
  }
}
/*
struct db_field_t
{
  field_type_t type;
  int length;
  char *prompt;
  char *name;
  int n_select;
  char **select_prompts;
  char **select_names;
  
};
*/

/**********************************************************************/
void gen_create_read_file_to_obj(FILE *fp_out, gen_names_t *gnames)
{
  fprintf(fp_out, "%s **%s::read(int *cnt, const char *home_dir)\n", 
       gnames->obj_type, gnames->obj_type);
  fprintf(fp_out, "{\n");
  fprintf(fp_out, " int max_points = 100;\n");
  fprintf(fp_out, " %s **dbps =\n",
       gnames->obj_type);
  fprintf(fp_out, " (%s **) malloc(max_points * sizeof(%s*));\n",
       gnames->obj_type, gnames->obj_type);
  fprintf(fp_out, " MALLOC_CHECK(dbps);\n");
  fprintf(fp_out, " *cnt = 0;\n");
  fprintf(fp_out, " int count = 0;\n");


  fprintf(fp_out, " char path[500];\n");
  //fprintf(fp_out, " safe_strcpy(path, home_dir, sizeof(path));\n");
  //fprintf(fp_out, " safe_strcat(path, \"/dbfiles/%s\", sizeof(path));\n",
   //   gnames->data_file_name);
  fprintf(fp_out, " snprintf(path, sizeof(path), \"%%s/%s\", home_dir);\n", gnames->relative_name);

  fprintf(fp_out, " FILE *fp = fopen(path, \"r\");\n");
  fprintf(fp_out, " if (fp == NULL)\n");
  fprintf(fp_out, " {\n");
  fprintf(fp_out, "   logfile->perror(path);\n");
  fprintf(fp_out, "   logfile->vprint(\"Can't open %%s\\n\", path);\n");
  fprintf(fp_out, "   return NULL;\n");
  fprintf(fp_out, " }\n");

  fprintf(fp_out, " char line[500];\n");
  fprintf(fp_out, " for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)\n");
  fprintf(fp_out, " {\n");
  fprintf(fp_out, "   char tmp[500];\n");
  fprintf(fp_out, "   int argc;\n");
  fprintf(fp_out, "   char *argv[50];\n");
  fprintf(fp_out, "   ltrim(line);\n");
  fprintf(fp_out, "   rtrim(line);\n");
  //fprintf(fp_out, "   safe_strcpy(tmp, (const char*) line, sizeof(tmp));\n");
  fprintf(fp_out, "   snprintf(tmp, sizeof(tmp), \"%%s\", line);\n");
  fprintf(fp_out, "   argc = get_delim_args(tmp, argv, '|', 50);\n");
  fprintf(fp_out, "   if (argc == 0)\n");
  fprintf(fp_out, "   {\n");
  fprintf(fp_out, "     continue;\n");
  fprintf(fp_out, "   }\n");
  fprintf(fp_out, "   else if (argv[0][0] == '#')\n");
  fprintf(fp_out, "   {\n");
  fprintf(fp_out, "     continue;\n");
  fprintf(fp_out, "   }\n");

  fprintf(fp_out, "   if (count >= max_points)\n");
  fprintf(fp_out, "   {\n");
  fprintf(fp_out, "     max_points += 50;\n");
  fprintf(fp_out, "     int new_size = max_points * sizeof(%s*);\n",
       gnames->obj_type);
  fprintf(fp_out, "     logfile->vprint(\"Reallocating: new size = %%d\\n\", new_size);\n");
  fprintf(fp_out, "     dbps = (%s **) realloc(dbps, new_size);\n",
       gnames->obj_type);
  fprintf(fp_out, "     MALLOC_CHECK(dbps);\n");
  fprintf(fp_out, "   }\n");

  fprintf(fp_out, "   logfile->vprint(\"%%s\\n\", line);\n");
  fprintf(fp_out, "   char errbuf[100];\n");
  fprintf(fp_out, "   dbps[count] = %s::create_one(argc, argv, errbuf, sizeof(errbuf));\n",
       gnames->obj_type);
  fprintf(fp_out, "   if (dbps[count] == NULL)\n");
  fprintf(fp_out, "   {\n");
  fprintf(fp_out, "     logfile->vprint(\"%%s:%%d\\n\", path, i+1);\n");
  fprintf(fp_out, "     logfile->vprint(\"%%s\\n\", errbuf);\n");
  fprintf(fp_out, "     continue;\n");
  fprintf(fp_out, "   }\n");
  fprintf(fp_out, "   count++;\n");

  fprintf(fp_out, " }\n");

  fprintf(fp_out, " if (count == 0)\n");
  fprintf(fp_out, " {\n");
  fprintf(fp_out, "   free(dbps);\n");
  fprintf(fp_out, "   return NULL;\n");
  fprintf(fp_out, " }\n");
  fprintf(fp_out, " *cnt = count;\n");
  fprintf(fp_out, " return dbps;\n");

  fprintf(fp_out, "}\n");
}

/**********************************************************************/
void gen_create_fields_to_obj(FILE *fp_out, gen_names_t *gnames)
{
  fprintf(fp_out, "\n\n/***************************/\n\n");
  //calc_point_t *calc_point_t::read_one(int argc, char *argv[], char *err, int esz);
  fprintf(fp_out, "%s *%s::create_one(int argc, char *argv[], char *err, int esz)\n", 
          gnames->obj_type, gnames->obj_type);
  fprintf(fp_out, "{\n");

  fprintf(fp_out, "  %s *objp;\n", gnames->obj_type);
  fprintf(fp_out, "  objp = new %s;\n", gnames->obj_type);
  fprintf(fp_out, "  if (objp == NULL)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    perror(\"new %s\");\n", gnames->obj_type);
  fprintf(fp_out, "    snprintf(err, esz, \"call to 'new' failed\");\n");
  fprintf(fp_out, "    return NULL;\n");
  fprintf(fp_out, "  }\n");
  
  fprintf(fp_out, "  if (argc != %d)\n", gnames->nf);
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    snprintf(err, esz, \"Wrong number of args for %s: %%d, should be %d\", argc);\n", gnames->table_name, gnames->nf);
  fprintf(fp_out, "    return NULL;\n");
  fprintf(fp_out, "  }\n");

  for (int i=0; i < gnames->nf; i++)
  {
    gen_create_one_field_to_obj(fp_out, gnames->dbfs[i], i);
  }

  fprintf(fp_out, "  objp->init_values();\n");
  fprintf(fp_out, "  return objp;\n");
  fprintf(fp_out, "}\n");
}

/**********************************************************************/

void gen_read_one_db(FILE *fp_out, gen_names_t *gnames)
{
  fprintf(fp_out, "    /*--------*/\n");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "  printf(\"---- Reading database '%s' -------------\\n\");", gnames->table_name);
  fprintf(fp_out, "\n");
  fprintf(fp_out, "%s\n", "  cbdata.first = false;");
  fprintf(fp_out, "%s\n", "  cbdata.n = 0;");
  fprintf(fp_out, "  cbdata.nf = %d;\n", gnames->nf);
  fprintf(fp_out, "  retval = sqlite3_exec(sqdb, \"select * from '%s';\",\n", gnames->table_name);
  fprintf(fp_out, "%s\n", "             my_callback, &cbdata, &errmsg);");
  fprintf(fp_out, "%s\n", "  if (retval != SQLITE_OK)");
  fprintf(fp_out, "%s\n", "  {");
  fprintf(fp_out, "%s\n", "    printf(\"Can not execute query, error = %d\\n\", retval);");
  fprintf(fp_out, "%s\n", "    if (errmsg != NULL)");
  fprintf(fp_out, "%s\n", "    {");
  fprintf(fp_out, "%s\n", "      printf(\"errmsg: %s\\n\", errmsg);");
  fprintf(fp_out, "%s\n", "      sqlite3_free(errmsg);");
  fprintf(fp_out, "%s\n", "    }");
  fprintf(fp_out, "%s\n", "    sqlite3_close(sqdb);");
  fprintf(fp_out, "%s\n", "    return 0;");
  fprintf(fp_out, "%s\n", "  }");
  fprintf(fp_out, "\n");

}


/**********************************************************************/

void gen_read_db(FILE *fp_out)
{
  fprintf(fp_out, "%s\n", "#include <stdio.h>");
  fprintf(fp_out, "%s\n", "#include <sqlite3.h>");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "%s\n", "struct callback_data_t");
  fprintf(fp_out, "%s\n", "{");
  fprintf(fp_out, "%s\n", "  bool first;");
  fprintf(fp_out, "%s\n", "  int nf;");
  fprintf(fp_out, "%s\n", "  int n;");
  fprintf(fp_out, "%s\n", "};");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "/**********************/\n");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "%s\n", "int my_callback(void *data, int n, char **vals, char **fields)");
  fprintf(fp_out, "%s\n", "{");
  fprintf(fp_out, "%s\n", "  callback_data_t *cbdata;");
  fprintf(fp_out, "%s\n", "  cbdata = (callback_data_t *) data; // how to pass data to callback.");
  fprintf(fp_out, "%s\n", "  cbdata->n++;");
  fprintf(fp_out, "%s\n", "  if (cbdata->first) printf(\"First callback!!!\\n\");");
  fprintf(fp_out, "%s\n", "  printf(\"Callback: %d, n = %d\\n\", cbdata->n, n);");
  fprintf(fp_out, "%s\n", "  if (n == cbdata->nf) printf(\"********** number of files MATCH!!\\n\");");
  fprintf(fp_out, "%s\n", "  for (int i=0; i < n; i++)");
  fprintf(fp_out, "%s\n", "  {");
  fprintf(fp_out, "%s\n", "    printf(\"%d: field: '%s', value: '%s'\\n\", i, fields[i], vals[i]);");
  fprintf(fp_out, "%s\n", "  }");
  fprintf(fp_out, "%s\n", "  cbdata->first = false;");
  fprintf(fp_out, "%s\n", "  return 0;");
  fprintf(fp_out, "%s\n", "}");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "/**********************/\n");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "%s\n", "int main(int argc, char *argv[])");
  fprintf(fp_out, "%s\n", "{");
  fprintf(fp_out, "%s\n", "  sqlite3 *sqdb;");
  fprintf(fp_out, "%s\n", "  sqlite3_stmt *stmt;");
  fprintf(fp_out, "%s\n", "  const char *tail;");
  fprintf(fp_out, "%s\n", "  int retval;");
  fprintf(fp_out, "%s\n", "  char *errmsg;");
  fprintf(fp_out, "%s\n", "  callback_data_t cbdata;");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "%s\n", "  retval = sqlite3_open_v2(\"react_def.db\",  &sqdb,");
  fprintf(fp_out, "%s\n", "     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);");
  fprintf(fp_out, "%s\n", "  if (retval != SQLITE_OK)");
  fprintf(fp_out, "%s\n", "  {");
  fprintf(fp_out, "%s\n", "    printf(\"Can not open db, error = %d\\n\", retval);");
  fprintf(fp_out, "%s\n", "    printf(\"Err = %s\\n\", sqlite3_errmsg(sqdb));");
  fprintf(fp_out, "%s\n", "    return 0;");
  fprintf(fp_out, "%s\n", "  }");
  fprintf(fp_out, "\n");
  fprintf(fp_out, "\n");

   /**
  for (int i=0; i < gnames->nf; i++)
  {
    gen_create_one_db(fp_out, "xxxx");
  }
  fprintf(fp_out, "%s\n", "}");
  **/
}

/**********************************************************************/


void gen_write_one(FILE *fp_out, gen_names_t *gnames)
{
  fprintf(fp_out, "\n\n/***************************/\n\n");
  fprintf(fp_out, "int write_one_%s(sqlite3 *sqdb, int argc, char *argv[])\n", 
          gnames->table_name);

  fprintf(fp_out, "{\n");
  //snprintf(str, sizeof(str), "insert into '%s' values(\n"); '%s',%d);",
  //         ddata[i].str, ddata[i].num);

  fprintf(fp_out, "  char qs[1000];");
  fprintf(fp_out, "  int retval;\n");
  fprintf(fp_out, "  char *errmsg;\n");

  fprintf(fp_out, "  if (argc < %d) // There must be at least %d args\n", gnames->nf, gnames->nf);
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    return -1;\n");
  fprintf(fp_out, "  }\n");
  char str[5000];
  bool first = true;
  snprintf(str, sizeof(str), 
   "  snprintf(qs, sizeof(qs), \"insert into \\\"%s\\\" values(", 
            gnames->table_name);
  for (int i=0; i < gnames->nf; i++)
  {
    char buf[200];
    if (!first) safe_strcat(str, ",", sizeof(str)); 
    safe_strcat(str, 
           gen_format(gnames->dbfs[i], buf, sizeof(buf)), sizeof(str));
    first = false;
  }
  safe_strcat(str, ");\"", sizeof(str)); 

  for (int i=0; i < gnames->nf; i++)
  {
    char buf[200];
    safe_strcat(str, 
        gen_value(gnames->dbfs[i], i, buf, sizeof(buf)), sizeof(str));
  }
  safe_strcat(str, ");", sizeof(str)); 
  fprintf(fp_out, "  %s\n", str);

  
  fprintf(fp_out, "  printf(\"qs =%%s\\n\", qs);\n");
  fprintf(fp_out, "  retval = sqlite3_exec(sqdb, qs,\n");
  fprintf(fp_out, "             NULL, NULL, &errmsg);\n");
  fprintf(fp_out, "  if (retval != SQLITE_OK)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    printf(\"qs = %%s\\n\", qs);\n");
  fprintf(fp_out, "    printf(\"Can not execute query, error = %%d\\n\", retval);\n");
  fprintf(fp_out, "    if (errmsg != NULL)\n");
  fprintf(fp_out, "    {\n");
  fprintf(fp_out, "      printf(\"errmsg: %%s\\n\", errmsg);\n");
  fprintf(fp_out, "      sqlite3_free(errmsg);\n");
  fprintf(fp_out, "    }\n");
  fprintf(fp_out, "    return -1;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  return 0;\n");

  fprintf(fp_out, "}\n");
}
  
/**********************************************************************/

void process_dat_file(FILE *fp_out, gen_names_t *gnames)
{
  char **argv;
  int argc;
  int line_num;

  delim_file_t df(300, 20, '|', '#');
  df.print_lines(true);


  for (argv = df.first(gnames->field_file_name, &argc, &line_num);
         (argv != NULL);
            argv = df.next(&argc, &line_num))
  {
    if (argc < 2)
    {
    }
  }
}

/**********************************************************************/

void gen_create_table(FILE *fp, gen_names_t *gnames)
{
  char fspec[100];
  char qs[5000];
  snprintf(qs, sizeof(qs), "create table %s(", gnames->table_name);

  int first = true;
  for (int i=0; i < gnames->nf; i++)
  {
    if (!first) safe_strcat(qs, ", ", sizeof(qs));
    safe_strcat(qs, field_spec(gnames->dbfs[i], fspec, sizeof(fspec)), sizeof(qs));
    first = false;
  }
  safe_strcat(qs, ", PRIMARY KEY (tag)", sizeof(qs));
  safe_strcat(qs, ");", sizeof(qs));

  fprintf(fp, "  retval = sqlite3_exec(sqdb,\n");
  fprintf(fp, "      \"%s\",\n", qs);
  fprintf(fp, "             NULL, NULL, &errmsg);\n");
  fprintf(fp, "  if (retval != SQLITE_OK)\n");
  fprintf(fp, "  {\n");
  fprintf(fp, "    printf(\"query = %s\\n\");\n", qs);
  fprintf(fp, "    printf(\"Can not execute query, error = %%d\\n\", retval);\n");
  fprintf(fp, "    if (errmsg != NULL)\n");
  fprintf(fp, "    {\n");
  fprintf(fp, "      printf(\"errmsg: %%s\\n\", errmsg);\n");
  fprintf(fp, "      sqlite3_free(errmsg);\n");
  fprintf(fp, "    }\n");
  fprintf(fp, "    sqlite3_close(sqdb);\n");
  fprintf(fp, "    return 0;\n");
  fprintf(fp, "  }\n");

}

/**********************************************************************/

void gen_insert_call(FILE *fp, gen_names_t *gnames)
{
  fprintf(fp, "  extern int write_one_%s(sqlite3 *sqdb, int argc, char *argv[]);\n", 
            gnames->table_name);
  fprintf(fp, "  insert_from_dat_file(sqdb, \"%s\", write_one_%s);\n", 
            gnames->data_file_name, gnames->table_name);
}

/**********************************************************************/

void gen_for_one_config(FILE *fp_out, FILE *fp_main, gen_names_t *gnames) 
{
  xprocess_file(fp_out, gnames);
  gen_create_table(fp_main, gnames);
  gen_insert_call(fp_main, gnames);
  gen_write_one(fp_out, gnames);
  //gen_read_dat(fp_out, gnames);
}

/**********************************************************************/

void  gen_sqlite_read_header(FILE *fp_out, const char *exe_name)
{
  fprintf(fp_out, "/***************************\n");
  fprintf(fp_out, "This is an auto-generated file, do NOT edit!!\n");
  fprintf(fp_out, "   generated by %s\n", exe_name);
  fprintf(fp_out, "***************************/\n");
  fprintf(fp_out, "#include <stdio.h>\n");
  fprintf(fp_out, "#include <stdlib.h>\n");
  fprintf(fp_out, "#include <sqlite3.h>\n");
  fprintf(fp_out, "#include \"rtcommon.h\"\n");
  fprintf(fp_out, "#include \"ap_config.h\"\n");
  fprintf(fp_out, "#include \"../db_point.h\"\n");
  fprintf(fp_out, "#include \"../logfile.h\"\n");
  fprintf(fp_out, "extern void sqlite_apconfig_read(sqlite3 *sqdb, ap_config_t *apconfig);\n");
  fprintf(fp_out, "static sqlite3 *sqdb = NULL;\n");
  fprintf(fp_out, "int read_fns_start_hook(const char *path)\n");
  fprintf(fp_out, "{\n");
  fprintf(fp_out, "  int retval;\n");
  fprintf(fp_out, "  const char * dbpath = \"./sqlite/react_def.db\";\n");
  fprintf(fp_out, "  logfile->vprint(\"sqlite start hook: %%s %%s\\n\", path, dbpath);\n");
  fprintf(fp_out, "  retval = sqlite3_open_v2(dbpath,  &sqdb,\n");
  fprintf(fp_out, "      SQLITE_OPEN_READONLY, NULL);\n");
  fprintf(fp_out, "  logfile->vprint(\"sqlite open return value: %%d\\n\", retval);\n");
  fprintf(fp_out, "  if (retval != SQLITE_OK)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    logfile->vprint(\"Can not open db, error = %%d\\n\", retval);\n");
  fprintf(fp_out, "    logfile->vprint(\"Err = %%s\\n\", sqlite3_errmsg(sqdb));\n");
  fprintf(fp_out, "    sqdb = NULL;\n");
  fprintf(fp_out, "    return -1;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  sqlite_apconfig_read(sqdb, &ap_config);\n");
  fprintf(fp_out, "  logfile->vprint(\"sqlite start hook: SUCCESS\\n\");\n");
  fprintf(fp_out, "  return 0;\n");

  fprintf(fp_out, "}\n");
  fprintf(fp_out, "void read_fns_end_hook(void)\n");
  fprintf(fp_out, "{\n");
  fprintf(fp_out, "  logfile->vprint(\"sqlite end hook ...\\n\");\n");
  fprintf(fp_out, "  sqlite3_close(sqdb);\n");
  fprintf(fp_out, "}\n");
}

/**********************************************************************/

void gen_for_one_sqlite_read(FILE *fp_out, gen_names_t *gnames) 
{
  fprintf(fp_out, "struct %s_cbdata_t\n", gnames->table_name); 
  fprintf(fp_out, "{\n");
    fprintf(fp_out, "   int max_points;\n");
    fprintf(fp_out, "   %s **dbps;\n", gnames->obj_type);
    fprintf(fp_out, "   int count;\n");
  fprintf(fp_out, "};\n");
    
  fprintf(fp_out, "int %s_callback(void *data, int n, char **vals, char **fields)\n", gnames->table_name);
  fprintf(fp_out, "{\n");
  fprintf(fp_out, "  %s_cbdata_t *cbdata =\n", gnames->table_name); 
  fprintf(fp_out, "    (%s_cbdata_t *) data;\n", gnames->table_name); 

  fprintf(fp_out, "  if (cbdata->count >= cbdata->max_points)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    cbdata->max_points += 50;\n");
  fprintf(fp_out, "    int new_size = cbdata->max_points * sizeof(%s*);\n",
       gnames->obj_type);
  fprintf(fp_out, "    logfile->vprint(\"Reallocating: new size = %%d\\n\", new_size);\n");
  fprintf(fp_out, "    cbdata->dbps = (%s **) realloc(cbdata->dbps, new_size);\n",
       gnames->obj_type);
  fprintf(fp_out, "    MALLOC_CHECK(cbdata->dbps);\n");
  fprintf(fp_out, "  }\n");


  fprintf(fp_out, "  for (int i=0; i < n; i++)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    logfile->vprint(\"%%s|\", vals[i] );\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  logfile->vprint(\"\\n\");\n");
  fprintf(fp_out, "  char errbuf[100];\n");
  fprintf(fp_out, "  cbdata->dbps[cbdata->count] =\n");
  fprintf(fp_out, "    %s::create_one(n, vals, errbuf, sizeof(errbuf));\n"
          , gnames->obj_type);
  fprintf(fp_out, "  if (cbdata->dbps[cbdata->count] == NULL)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    logfile->vprint(\"  ERROR: table: %s\\n\");\n", 
            gnames->table_name);
  fprintf(fp_out, "    logfile->vprint(\"  %%s\\n\", errbuf);\n");
  fprintf(fp_out, "    return 0;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  cbdata->count++;\n");
 
  fprintf(fp_out, "  return 0;\n");
  fprintf(fp_out, "}\n");

  fprintf(fp_out, "%s **%s::read(int *cnt, const char *home_dir)\n",
       gnames->obj_type, gnames->obj_type);
  fprintf(fp_out, "{\n"); 
  fprintf(fp_out, "  %s_cbdata_t cbdata;\n", gnames->table_name); 
  fprintf(fp_out, "  char *errmsg;\n");
  fprintf(fp_out, "  int retval;\n");
  fprintf(fp_out, "  cbdata.max_points = 100;\n");
  fprintf(fp_out, "  logfile->vprint(\"sqlite read from table: %s\\n\");\n", gnames->table_name);
  fprintf(fp_out, "  cbdata.dbps =\n");
  fprintf(fp_out, "     (%s **) malloc(cbdata.max_points * sizeof(%s*));\n",
       gnames->obj_type, gnames->obj_type);
  fprintf(fp_out, "  MALLOC_CHECK(cbdata.dbps);\n");
  fprintf(fp_out, "  *cnt = 0;\n");
  fprintf(fp_out, "  cbdata.count = 0;\n");
  fprintf(fp_out, "  retval = sqlite3_exec(sqdb, \"select * from '%s';\",\n", gnames->table_name);
  fprintf(fp_out, "           %s_callback, &cbdata, &errmsg);\n", 
               gnames->table_name);
  fprintf(fp_out, "  if (retval != SQLITE_OK)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    logfile->vprint(\"Can not execute query, error = %%d\\n\", retval);\n");
  fprintf(fp_out, "    if (errmsg != NULL)\n");
  fprintf(fp_out, "    {\n");
  fprintf(fp_out, "      logfile->vprint(\"errmsg: %%s\\n\", errmsg);\n");
  fprintf(fp_out, "      sqlite3_free(errmsg);\n");
  fprintf(fp_out, "    }\n");
  fprintf(fp_out, "    //sqlite3_close(sqdb);\n");
  fprintf(fp_out, "    return NULL;\n");
  fprintf(fp_out, "  }\n");
  fprintf(fp_out, "  if (cbdata.count == 0)\n");
  fprintf(fp_out, "  {\n");
  fprintf(fp_out, "    free(cbdata.dbps);\n");
  fprintf(fp_out, "    return NULL;\n");
  fprintf(fp_out, "  }\n");

  fprintf(fp_out, "  *cnt = cbdata.count;\n");
  fprintf(fp_out, "  return cbdata.dbps;\n");

  fprintf(fp_out, "}\n");
}

/**********************************************************************/

int main(int argc, char *argv[])
{
  char *dir_name;
  DIR *dir;
  struct dirent *dent;
  char base_name[100];
  char relative_name[100];
  char config_name[100];
  char dat_name[100];
  //char base_dat_name[100];
  struct gen_names_t gnames;

  if (argc < 2)
  {
    printf("You must specify a directory\n");
    return -1;
  }

  dir_name = argv[1];

  dir = opendir(dir_name);

  if (dir == NULL)
  {
    perror(dir_name);
    return -1;
  }

  const char *out_temp = "write_one_fns.cpp";
  FILE *fp_out = fopen(out_temp, "w");
  if (fp_out == NULL)
  {
    perror(out_temp);
    exit(0);
  }

  const char *out_main = "outmain.cpp";
  FILE *fp_main = fopen(out_main, "w");
  if (fp_main == NULL)
  {
    perror(out_main);
    exit(0);
  }

  const char *sqlite_read_fns = "sqlite_read_fns.cpp";
  FILE *fp_sqlite_read = fopen(sqlite_read_fns, "w");
  if (fp_sqlite_read == NULL)
  {
    perror(sqlite_read_fns);
    exit(0);
  }

  const char *out_obj = "read_fns.cpp";
  FILE *fp_out_obj = fopen(out_obj, "w");
  if (fp_out_obj == NULL)
  {
    perror(out_obj);
    exit(0);
  }

  const char *create_fns = "create_fns.cpp";
  FILE *fp_create_fns = fopen(create_fns, "w");
  if (fp_create_fns == NULL)
  {
    perror(create_fns);
    exit(0);
  }

  const char *read_db = "read_db.cpp";
  FILE *fp_read_db = fopen(read_db, "w");
  if (fp_read_db == NULL)
  {
    perror(read_db);
    exit(0);
  }

  gen_read_db(fp_read_db);

  gen_header(fp_main, argv[0]);
  write_main_start(fp_main);

  gen_header(fp_out, argv[0]);
  gen_field_header(fp_out_obj, argv[0]);
  gen_field_header(fp_create_fns, argv[0]);
  gen_sqlite_read_header(fp_sqlite_read, __FILE__);

  fprintf(fp_out_obj, "int read_fns_start_hook(const char *path)\n");
  fprintf(fp_out_obj, "{\n");
  fprintf(fp_out_obj, "  logfile->vprint(\"Delimited text file start hook: %%s\\n\", path);\n");
  fprintf(fp_out_obj, "  return 0;\n");
  fprintf(fp_out_obj, "}\n");
  fprintf(fp_out_obj, "void read_fns_end_hook(void)\n");
  fprintf(fp_out_obj, "{\n");
  fprintf(fp_out_obj, "  logfile->vprint(\"Delimited text file end hook\\n\");\n");
  fprintf(fp_out_obj, "}\n");
  
  dent = readdir(dir);
  while (dent != NULL)
  {
    int len = strlen(dent->d_name);
    if (0 == strncmp(".config", dent->d_name + len - 7, 7)) 
    {
      snprintf(config_name, sizeof(config_name), "%s/%s", 
                    dir_name, dent->d_name);
      snprintf(base_name, sizeof(base_name), "%s", dent->d_name);
      base_name[len-7] = '\0';
      snprintf(relative_name, sizeof(relative_name), "/dbfiles/%s.dat", base_name);
      snprintf(dat_name, sizeof(dat_name), "%s/%s.dat", 
                    dir_name, base_name);
      printf("%s\n", base_name); 
      printf("%s\n", config_name); 
      printf("%s\n", dat_name); 
      gnames.field_file_name = config_name;
      gnames.data_file_name = dat_name;
      gnames.base_name = base_name;
      gnames.relative_name = relative_name;
      gen_for_one_config(fp_out, fp_main, &gnames);
      printf("Calling gen_create_fields_to_obj()\n"); 
      gen_create_fields_to_obj(fp_create_fns, &gnames);
      printf("Calling gen_create_read_files_to_obj()\n"); 
      //gen_create_read_file_to_obj(stderr, &gnames);
      gen_create_read_file_to_obj(fp_out_obj, &gnames);
      gen_read_one_db(fp_read_db, &gnames);
      gen_for_one_sqlite_read(fp_sqlite_read, &gnames); 
    }
    dent = readdir(dir);
  }
  write_main_end(fp_main);

  fprintf(fp_read_db, "}\n");

  fclose(fp_out);
  fclose(fp_main);
  fclose(fp_out_obj);
  fclose(fp_read_db);
  fclose(fp_create_fns);
  fclose(fp_sqlite_read);
}

/**********************************************************************/
