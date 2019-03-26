
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "rtcommon.h"
#include "arg.h"

#include "msg_queue.h"
#include "tcontrol.h"
#include "event_alg.h"
#include "section_reader.h"
#include "trainpos.h"
#include "dc_evalg.h"

/********************************************************/
display_reader_t::display_reader_t(void)
{
  n_displays = 0;
}

/********************************************************/

const display_info_t *display_reader_t::get_display_data(unsigned n)
{
  if (n >= n_displays)
  {
    return NULL;
  }
  return displays[n];
}

/********************************************************/

void display_reader_t::read_file(const char *fname)
{
  char line[300];

  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    exit(0);
  }

  n_displays = 0;
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, ',', 20);
    if (argc == 0)
    {
      continue;
    }
    if (argv[0][0] == '#')
    {
      continue;
    }
    if (argc != 11)
    {
      printf("Wrong number of args: %d, should be 11\n", argc);
      continue;
    }
    printf("%s\n", line);
    display_info_t *dd = new display_info_t;
    
    safe_strcpy(dd->background, argv[0], sizeof(dd->background));
    safe_strcpy(dd->square, argv[1], sizeof(dd->square));
    safe_strcpy(dd->square_unexpected, argv[2], sizeof(dd->square_unexpected));
    safe_strcpy(dd->html_out, argv[3], sizeof(dd->html_out));
    dd->x1 = atol(argv[4]);
    dd->x2 = atol(argv[5]);
    dd->y1 = atol(argv[6]);
    dd->y2 = atol(argv[7]);
    dd->text_offset_y1 = atol(argv[8]);
    dd->text_offset_y2 = atol(argv[9]);
    dd->n_sections = atol(argv[10]);
    displays[n_displays] = dd;
    n_displays++;
    if (n_displays >= (sizeof(displays)/sizeof(displays[0])))
    {
      printf("Error: Maximum reached\n");
      break;
    }
  }
}

/********************************************************

int main(int argc, char *argv[])
{
  display_reader_t reader;   
  reader.read_file("display_info.txt");
  int n = reader.get_n_displays();
  for (int i=0; i < n; i++)
  {
    const display_info_t *dd = reader.get_display_data(i);
    printf("%s, %s, %s, %s, %d, %d, %d, %d, %d\n", 
      dd->background, 
      dd->square,
      dd->square_unexpected, 
      dd->html_out, 
      dd->x1,
      dd->x2,
      dd->y1,
      dd->y2,
      dd->n_sections);
  } 
}
********/
