

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtcommon.h"

int section_times[] = { 118, 104, 77, 89, 72, 86, 70, 91, 101, 72, 80, 87, 92, 80, 98, 103, 81, 130,
                119, 102, 75, 87, 78, 83, 77, 98, 112, 84, 87, 81, 95, 77, 95, 113, 88, 121};

const char *timetable[] = {
"A1	5:00:00", 
"A4	5:15:00", 
"A7	5:30:00", 
"A10	5:45:00", 
"B1	6:00:00", 
"B2	6:05:15", 
"B3	6:10:30", 
"B4	6:15:45", 
"B5	6:21:00", 
"B6	6:26:15", 
"B7	6:31:30", 
"B8	6:36:45", 
"B9	6:42:00", 
"B10	6:47:15", 
"B11	6:52:30", 
"B12	6:57:45", 
"C1	7:03:00", 
"C2	7:08:15", 
"C3	7:13:30", 
"C4	7:18:45", 
"C5	7:24:00", 
"C6	7:29:15", 
"C7	7:34:30", 
"C8	7:39:45", 
"C9	7:45:00", 
"C10	7:50:15", 
"C11	7:55:30", 
"C12	8:00:45", 
"D1	8:06:00", 
"D2	8:11:15", 
"D3	8:16:30", 
"D4	8:21:45", 
"D5	8:27:00", 
"D6	8:32:15", 
"D7	8:37:30", 
"D8	8:42:45", 
"D9	8:48:00", 
"D10	8:53:15", 
"D11	8:58:30", 
"D12	9:03:45", 
"E1	9:09:00", 
"E2	9:14:15", 
"E3	9:19:30", 
"E4	9:24:45", 
"E5	9:30:00", 
"E6	9:39:00", 
"E8	9:48:00", 
"E10	9:57:00", 
"E11	10:06:00", 
"F1	10:15:00", 
"F3	10:24:00", 
"F5	10:33:00", 
"F6	10:42:00", 
"F8	10:51:00", 
"F10	11:00:00", 
"F11	11:09:00", 
"G1	11:18:00", 
"G3	11:27:00", 
"G5	11:36:00", 
"G6	11:45:00", 
"G8	11:54:00", 
"G10	12:03:00", 
"G11	12:10:00", 
"G12	12:17:00", 
"H1	12:24:00", 
"H3	12:31:00", 
"H4	12:38:00", 
"H5	12:45:00", 
"H6	12:52:00", 
"H8	12:59:00", 
"H10	13:06:00", 
"H11	13:13:00", 
"H12	13:20:00", 
"I1	13:27:00", 
"I3	13:34:00", 
"I4	13:41:00", 
"I5	13:48:00", 
"I6	13:55:00", 
"I8	14:02:00", 
"I10	14:09:00", 
"I11	14:16:00", 
"I12	14:23:00", 
"J1	14:30:00", 
"J3	14:37:00", 
"J4	14:44:00", 
"J5	14:51:00", 
"J6	14:58:00", 
"J8	15:05:00", 
"J10	15:12:00", 
"J11	15:19:00", 
"J12	15:26:00", 
"K1	15:33:00", 
"K3	15:40:00", 
"K4	15:47:00", 
"K5	15:54:00", 
"K6	16:01:00", 
"K8	16:08:00", 
"K10	16:15:00", 
"K11	16:22:00", 
"K12	16:29:00", 
"L1	16:36:00", 
"L3	16:43:00", 
"L4	16:50:00", 
"L5	16:57:00", 
"L6	17:04:00", 
"L7	17:09:15", 
"L8	17:14:30", 
"L9	17:19:45", 
"L10	17:25:00", 
"L11	17:30:15", 
"L12	17:35:30", 
"M1	17:40:45", 
"M2	17:46:00", 
"M3	17:51:15", 
"M4	17:56:30", 
"M5	18:01:45", 
"M6	18:07:00", 
"M7	18:12:15", 
"M8	18:17:30", 
"M9	18:22:45", 
"M10	18:28:00", 
"M11	18:33:15", 
"M12	18:38:30", 
"N1	18:43:45", 
"N2	18:49:00", 
"N3	18:54:15", 
"N4	18:59:30", 
"N5	19:04:45", 
"N6	19:10:00", 
"N7	19:15:15", 
"N8	19:20:30", 
"N9	19:25:45", 
"N10	19:31:00", 
"N11	19:36:15", 
"N12	19:41:30", 
"O1	19:46:45", 
"O2	19:52:00", 
"O3	19:57:15", 
"O4	20:02:30", 
"O5	20:07:45", 
"O6	20:13:00", 
"O7	20:18:15", 
"O8	20:23:30", 
"O9	20:28:45", 
"O10	20:34:00", 
"O12	20:44:30", 
"P2	20:55:00", 
"P4	21:05:30", 
"P6	21:16:00", 
"P8	21:26:30", 
"P10	21:37:00", 
"P12	21:47:30", 
"Q2	21:58:00", 
"Q4	22:08:30", 
"Q6	22:20:00", 
"Q8	22:32:00"
};

/***************************************/

int main(int argc, char *argv[])
{
  const char *dir;
  char template_file[200];
  char config_file[200];
  char sections_file[200];
  char timetable_file[200];
  char display_file[200];
  char bg_file[200];
  char sq_file[200];
  char ux_file[200];
  char line[300];
  if (argc < 2)
  {
    printf("must enter file name\n");
    exit(0);
  } 
  
  dir = "";
  if (argc > 2)
  {
    dir = argv[2];
  }

  const char *prefix = argv[1];

  snprintf(template_file, sizeof(template_file), "%s%s_template.txt", dir, prefix);
  snprintf(config_file, sizeof(config_file), "%s%s_config.txt", dir, prefix);
  snprintf(sections_file, sizeof(sections_file), "%s%s_sections.txt", dir, prefix);
  snprintf(timetable_file, sizeof(timetable_file), "%s%s_tt.txt", dir, prefix);
  snprintf(display_file, sizeof(display_file), "%s%s_display.txt", dir, prefix);
  snprintf(bg_file, sizeof(bg_file), "%s%s_1024.png", dir, prefix);
  snprintf(sq_file, sizeof(sq_file), "%s%s_square.png", dir, prefix);
  snprintf(ux_file, sizeof(ux_file), "%s%s_unexpected.png", dir, prefix);

  FILE *fp = fopen(template_file, "r");
  if (fp == NULL)
  {
    perror(template_file);
    exit(0);
  }

  char *stations[200];
  int n_stations = 0;
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    ltrim(line);
    rtrim(line);
    stations[i] = strdup(line);
    printf("%d: %s\n", i+1, stations[i]);
    n_stations++;
  }
  printf("found %d stations\n", n_stations);
  fclose(fp);

  fp = fopen(sections_file, "w");
  if (fp == NULL)
  {
    perror(sections_file);
    exit(0);
  }
  for (int i=0; i < (n_stations - 1); i++)
  {
    printf("%s\t%d\t%d\t%d\n", stations[i], section_times[i], 5, section_times[i] - 5);
    fprintf(fp, "%s\t%d\t%d\t%d\n", stations[i], section_times[i], 5, section_times[i] - 5);
  }
  for (int i = (n_stations - 1); i > 0; i--)
  {
    printf("%s\t%d\t%d\t%d\n", stations[i], section_times[i-1], 5, section_times[i-1] - 5);
    fprintf(fp, "%s\t%d\t%d\t%d\n", stations[i], section_times[i-1], 5, section_times[i-1] - 5);
  }
  fclose(fp);

  fp = fopen(timetable_file, "w");
  if (fp == NULL)
  {
    perror(timetable_file);
    exit(0);
  }

  for (int i=0; i < int(sizeof(timetable)/sizeof(timetable[0])); i++)
  {
    printf("%d: %s\n", i, timetable[i]);
    fprintf(fp, "%s\n", timetable[i]);
  }

  fclose(fp);

  fp = fopen(config_file, "w");
  if (fp == NULL)
  {
    perror(config_file);
    exit(0);
  }

  fprintf(fp, "SECTIONS_FILE,%s\n", sections_file);
  fprintf(fp, "TIMETABLE_FILE,%s\n", timetable_file);
  fprintf(fp, "DISPLAY_FILE,%s\n", display_file);
  fprintf(fp, "HTML_HOME,./");
 
  fclose(fp);

  fp = fopen(display_file, "w");
  if (fp == NULL)
  {
    perror(display_file);
    exit(0);
  }

  fprintf(fp, "%s,%s,%s,Line1.html,39,975,42,68,-26,19,%d", bg_file, sq_file, ux_file, (n_stations - 1) * 2); 

  fclose(fp);

//Line1_1024.png,square10x10.png,square_unexpected.png,Line1.html, 26, 967, 45, 72, -26, 19, 36


}
