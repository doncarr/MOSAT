

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"


void process_file(const char *fname, FILE *fp_out)
{
  char **argv;
  int argc;
  char **label_argv;
  int label_argc;
  char **value_argv;
  int value_argc;
  int line_num;

  delim_file_t df(300, 20, '|', '#');
  df.print_lines(true);

  delim_separator_t ds_label(100, 20, ',');
  delim_separator_t ds_value(100, 20, ',');

  fprintf(fp_out, "<html>\n");
  fprintf(fp_out, "<head>\n");
  fprintf(fp_out, "<title> artc_form.php </title>\n");
  fprintf(fp_out, "<body style=\"color: rgb(0, 0, 0); background-color: rgb(128, 128, 128);\"\n");
  fprintf(fp_out, " link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">\n");

  fprintf(fp_out, "</head>\n");
  fprintf(fp_out, "<body>\n");
  fprintf(fp_out, "<FORM method=\"POST\" action=\"\">\n");


  fprintf(fp_out, "<table width=\"100%%\">");

  for (argv = df.first(fname, &argc, &line_num);
         (argv != NULL); 
            argv = df.next(&argc, &line_num))
  {
    if (argc < 2)
    {
      printf("Wrong number of args, line %d\n", line_num);
      continue;
    }
    printf("%d: %s", line_num, argv[0]);
    for (int i=1; i < argc; i++)
    {
      printf(", %s", argv[i]);
    }  
    printf("\n");
    if (0 == strcasecmp(argv[0], "table"))
    {
      fprintf(fp_out, "<h1>%s</h1>\n", argv[1]);
      //fprintf(fp_out, "<i>Campos marcados con un asterisco son obligatorios</i>\n");
//      fprintf(fp_out, "<hr><br>\n");
    }
    else if (0 == strcasecmp(argv[0], "separator"))
    {
//      fprintf(fp_out, "</table>\n");
//      fprintf(fp_out, "<br><br>\n");
      fprintf(fp_out, "<tr><td colspan=\"3\"><hr></td></tr>\n");
      fprintf(fp_out, "<tr><td colspan=\"3\"><h2>%s<h2></td></tr>\n", argv[1]);
//      fprintf(fp_out, "<table width=\"100%%\">");
    }
    else if (0 == strcasecmp(argv[0], "string"))
    {
      fprintf(fp_out, "<tr>\n");
      fprintf(fp_out, "  <td class=\"name\">%s: </td>\n", argv[1]);
      fprintf(fp_out, "  <td class=\"input\">\n");
      fprintf(fp_out, "    <input TYPE=\"text\" name=\"string_%s\" size=\"%s\" value=\"\">\n", argv[2], argv[3]);
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "  <td> \n");
      fprintf(fp_out, "    4 a %s caracteres \n", argv[3]);
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "</tr>\n");
    }
    else if (0 == strcasecmp(argv[0], "double"))
    {
      fprintf(fp_out, "<tr>\n");
      fprintf(fp_out, "  <td class=\"name\">%s: </td>\n", argv[1]);
      fprintf(fp_out, "  <td class=\"input\">\n");
      fprintf(fp_out, "    <input TYPE=\"text\" name=\"double_%s\" size=\"12\" value=\"\">\n", argv[2]);
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "  <td> \n");
      fprintf(fp_out, "    double\n");
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "</tr>\n");
    }
    else if (0 == strcasecmp(argv[0], "int"))
    {
      fprintf(fp_out, "<tr>\n");
      fprintf(fp_out, "  <td class=\"name\">%s: </td>\n", argv[1]);
      fprintf(fp_out, "  <td class=\"input\">\n");
      fprintf(fp_out, "    <input TYPE=\"text\" name=\"int_%s\" size=\"8\" value=\"\">\n", argv[2]);
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "  <td> \n");
      fprintf(fp_out, "    int\n");
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "</tr>\n");
    }
    else if (0 == strcasecmp(argv[0], "bool"))
    {
      fprintf(fp_out, "<tr>\n");
      fprintf(fp_out, "  <td class=\"name\">%s</td>\n", argv[1]);
      fprintf(fp_out, "  <td class=\"input\">\n");
      fprintf(fp_out, "    <input type=\"checkbox\" name=\"bool_%s\">(Enable/Disable)<br>\n", argv[2]);
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "  <td> \n");
      fprintf(fp_out, "   <!-- No comment -->\n");
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "</tr>\n");
    }
    else if (0 == strcasecmp(argv[0], "dropdown"))
    {
      if (argc != 5)
      {
        printf("Wrong number of args for dropdown, line %d\n", line_num);
        continue;
      }
      printf("dropdown items: %s, %s\n", argv[3], argv[4]);
      label_argv = ds_label.separate(&label_argc, argv[3]); 
      value_argv = ds_value.separate(&value_argc, argv[4]); 
      if (label_argc != value_argc)
      {
        printf("The labels and values must have the same number of items, line %d\n", line_num);
        continue;
      }
      fprintf(fp_out, "<tr>\n");
      fprintf(fp_out, "  <td class=\"name\">%s</td>\n", argv[1]);
      fprintf(fp_out, "  <td class=\"input\">\n");
      fprintf(fp_out, "    <select name=\"list_%s\">\n", argv[2]);
      fprintf(fp_out, "	 <option name=\"unset\" value=\"\" selected></option>\n");

      //fprintf(fp_out, "	 <option value=\"M\">Masculino</option>\n");
      //fprintf(fp_out, "	 <option value=\"F\">Femenino</option>\n");
      for (int j=0; j < label_argc; j++)
      {
        fprintf(fp_out, "	 <option value=\"%s\">%s</option>\n", 
                         value_argv[j], label_argv[j]);
      }

      fprintf(fp_out, "    </select>\n");
      fprintf(fp_out, "  </td>\n");
      fprintf(fp_out, "</tr>\n");
    }
    else
    {
      printf("Error - unknown field type: %s\n", argv[0]);
      exit(0);
    }
  }
  fprintf(fp_out, "<tr><td colspan=\"3\"><hr></td></tr>\n");
  fprintf(fp_out, "</table>\n");
  fprintf(fp_out, "<br><br>\n");
  fprintf(fp_out, "<center>\n");
  fprintf(fp_out, "  <input type=\"submit\" name=\"submit\" value=\"Submit\">\n");
  fprintf(fp_out, "  <input type=\"button\" value=\"Cancelar\" onClick=location.href=\"../\">\n");
  fprintf(fp_out, "</center>\n");

  fprintf(fp_out, "</form>\n");
  fprintf(fp_out, "</body>\n");
  fprintf(fp_out, "</html>\n");
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("You must specify the file name - exiting . . .\n");
    exit(0);
  }
  const char *fname = argv[1];
  const char *out_fname = "out.html";
  FILE *fp_out = fopen(out_fname, "w");
  if (fp_out == NULL)
  {
    perror(out_fname);
    exit(0);
  }
  process_file(fname, fp_out);
  fclose(fp_out);
}
