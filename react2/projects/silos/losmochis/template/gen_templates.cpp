
#include <stdio.h>
#include <stdlib.h>

void gen_one(const char *file_name, int ubicacion, int silo, int column, int n, const char *title)
{
  FILE *fp = fopen(file_name, "w");

  fprintf(fp, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n");
  fprintf(fp, "<HTML>\n");
  fprintf(fp, "<HEAD>\n");
  fprintf(fp, "<META HTTP-EQUIV=\"CONTENT-TYPE\" CONTENT=\"text/html; charset=utf-8\">\n");
  fprintf(fp, "<meta http-equiv=\"refresh\" content=\"5\">\n");
  fprintf(fp, "<meta http-equiv=\"pragma\" content=\"no-cache\">\n");
  fprintf(fp, "</HEAD>\n");
  fprintf(fp, "<BODY LANG=\"en-US\">\n");
  fprintf(fp, "<body style=\"color: rgb(0, 0, 0); background-color: rgb(150, 150, 150);\"\n");
  fprintf(fp, " link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">\n");
  fprintf(fp, "\n");
  fprintf(fp, "<big><strong>Ultima Actualizaci&oacute;n: $$$$TIME(                                                                              )</big></strong><br>\n");
  fprintf(fp, "\n");
  fprintf(fp, "<br><big><big>%s</big></big><br><br>\n", title);
  fprintf(fp, "\n");

  for (int i=0; i < n; i++)
  {
    fprintf(fp, "Temp %d   : $$$$AI(U%d_S%d_C%d_TEMP_%d) °C<br>\n", i + 1, ubicacion, silo, column, i + 1);
  }

  for (int i=0; i < n; i++)
  {
    fprintf(fp, "\n\n");
    fprintf(fp, "$$$$AI(U%d_S%d_C%d_TEMP_%d|bar_small.png|small_half_off.png|small_half_on.png|100|%d|200|0|50                                                                                                                                                                                )\n", 
         ubicacion, silo, column, i + 1, 250 + (i * 50));

    fprintf(fp, "<div style=\"position: absolute; top: 310px; left: %dpx;\">\n", 250 + (i * 50));
    fprintf(fp, "$$$$AI(U%d_S%d_C%d_TEMP_%d)\n", ubicacion, silo, column, i + 1);
    fprintf(fp, "</div>\n");
  }
  fprintf(fp, "\n\n");

  int left = 220 + (n * 50);
  fprintf(fp, "<div style=\"position: absolute; top: 290px; left: %dpx;\">\n", left);
  fprintf(fp, "--- 0 °C\n");
  fprintf(fp, "</div>\n");
  fprintf(fp, "<div style=\"position: absolute; top: 250px; left: %dpx;\">\n", left);
  fprintf(fp, "--- 10 °C\n");
  fprintf(fp, "</div>\n");
  fprintf(fp, "<div style=\"position: absolute; top: 210px; left: %dpx;\">\n", left);
  fprintf(fp, "--- 20 °C\n");
  fprintf(fp, "</div>\n");
  fprintf(fp, "<div style=\"position: absolute; top: 170px; left: %dpx;\">\n", left);
  fprintf(fp, "--- 30 °C\n");
  fprintf(fp, "</div>\n");
  fprintf(fp, "<div style=\"position: absolute; top: 130px; left: %dpx;\">\n", left);
  fprintf(fp, "--- 40 °C\n");
  fprintf(fp, "</div>\n");
  fprintf(fp, "<div style=\"position: absolute; top: 90px; left: %dpx;\">\n", left);
  fprintf(fp, "--- 50 °C\n");
  fprintf(fp, "</div>\n");
  fprintf(fp, "\n\n");

  for (int i=0; i < n; i++)
  {
    fprintf(fp, "<div style=\"position: absolute; top: 70px; left: %dpx;\">\n", 250 + (i * 50));
    fprintf(fp, "%d\n", i + 1);
    fprintf(fp, "</div>\n");
  }

  fprintf(fp, "</BODY>\n");
  fprintf(fp, "</HTML>\n");
}


int main(int argc, char *argv[])
{
  if (argc < 6)
  {
    printf("You must enter filename, location number, silo number, column number, number of sensors\n"); 
    return -1;
  }
  char fname[200];
  char title[200];
  const char *file_prefix = argv[1];
  int ubic = atoi(argv[2]);
  int silo = atoi(argv[3]);
  int col = atoi(argv[4]);
  int n_temp = atoi(argv[5]);

  snprintf(fname, sizeof(fname), "%s_U%d_S%d_C%d.html", file_prefix, ubic, silo, col);
  snprintf(title, sizeof(title), "Ubicación %d Silo %d Columna %d", ubic, silo, col);
  
  gen_one(fname, ubic, silo, col, n_temp, title);

}


