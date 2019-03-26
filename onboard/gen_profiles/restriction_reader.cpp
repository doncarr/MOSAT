
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtcommon.h"
#include "arg.h"

#include "restriction_reader.h"

/*******************************************************/

restriction_reader_t::restriction_reader_t(void)
{
  n_restrictions = 0;
}

/*******************************************************/

void restriction_reader_t::print_restriction(const restriction_def_t *res)
{
  printf("%s: start: %lf, end: %lf, max speed: %lf, normal stop point: %lf\n", 
       restriction_string(res->type), res->start, res->end, res->max_speed, res->normal_stop_point);
}

/*******************************************************/

const char *restriction_reader_t::restriction_string(restriction_type_t type)
{
  switch (type)
  {
    case RESTRICT_CURVE: return "CURVE";
    case RESTRICT_OTHER: return "OTHER";
    case RESTRICT_STATION: return "STATION";
    case RESTRICT_CROSSING: return "CROSSING";
    case RESTRICT_CONSTRUCTION: return "CONSTRUCTION";
    case RESTRICT_SECTION: return "SECTION";
    case RESTRICT_MAINTENANCE: return "MAINTENANCE";
    case RESTRICT_ACCEL_DECEL_CONTROL: return "AccelDecelControl";
    case RESTRICT_MOD_NEXT: return "ModNext";
  }
  return "ERROR";
}

/*******************************************************/

void remove_commas(char *str)
{
  char *p, *q;
  for(p = str; *p != '\0'; p++)
  {
    if (*p == ',')
    {
       for (q = p; *q != '\0'; q++)
       {
         *q = *(q+1);
       }
       p--; // back up, there could be two commas in a row
    }
  }
}

/*******************************************************/

int restriction_reader_t::read_restrictions(const char *fname)
{
  int argc, line_num;
  char **argv;
  n_restrictions = 0;
  bool mod_next = false;
  restriction_def_t modifier;
  delim_file_t df(600, 50, '\t', '#');
  
  double last_start = -100.0;
  argv = df.first(fname, &argc, &line_num);
  for (int i=0; (argv != NULL) && (i < RT_MAX_RESTRICTIONS); argv = df.next(&argc, &line_num))
  {
    if (argv == NULL)
    {
      break;
    }
    if (argv[0][0] == '#')
    {
      printf("-- Skipping comment line '#'\n");
      continue;
    }
    if (0 == strncmp(argv[0], "\"#", 2))
    {
      printf("-- Skipping comment line '\"#'\n");
      continue;
    }
    if (argc < 6)
    {
      printf("*********** Wrong number of args: %d, line %d\n", argc, line_num);
      exit(0);
    }
    printf("%d: %s, %s, %s, %s, %s, %s\n", i, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
    /* Some spread sheeds will put quotes around the strings, we must get rid of the quotes*/
    strip_quotes(argv[0]);
    strip_quotes(argv[1]);
    /* Some spread sheets put commas every 3 digits, but, atof() stops scanning when it reaches a comma */
    /* we must get rid of the commas */
    remove_commas(argv[2]);
    remove_commas(argv[3]);
    remove_commas(argv[4]);
    remove_commas(argv[5]);
    const char *tp_string = argv[0];
    safe_strcpy(restriction[i].description, argv[1], sizeof(restriction[i]));
    restriction[i].start = atof(argv[2]);
    restriction[i].end = atof(argv[3]);
    restriction[i].normal_stop_point = atof(argv[4]);
    restriction[i].max_speed = atof(argv[5]);
    if (0 == strcasecmp(tp_string, "CURVE"))
    {
      restriction[i].type = RESTRICT_CURVE;
    }
    else if (0 == strcasecmp(tp_string, "STATION"))
    {
      restriction[i].type = RESTRICT_STATION;
      if ((restriction[i].start > restriction[i].normal_stop_point) || (restriction[i].end < restriction[i].normal_stop_point))
       {
         printf("******* Invalid, normal stop point must be between start and end of platform, line %d\n", line_num);
       }
    }
    else if (0 == strcasecmp(tp_string, "CROSSING"))
    {
      restriction[i].type = RESTRICT_CROSSING;
    }
    else if (0 == strcasecmp(tp_string, "CONSTRUCTION"))
    {
      restriction[i].type = RESTRICT_CONSTRUCTION;
    }
    else if (0 == strcasecmp(tp_string, "OTHER"))
    {
      restriction[i].type =  RESTRICT_OTHER;
    }
    else if (0 == strcasecmp(tp_string, "MAINTENANCE"))
    {
      restriction[i].type = RESTRICT_MAINTENANCE;
    }
    else if (0 == strcasecmp(tp_string, "SECTION"))
    {
      restriction[i].type = RESTRICT_SECTION;
    }
    else if (0 == strcasecmp(tp_string, "AccelDecelControl"))
    {
      restriction[i].type = RESTRICT_ACCEL_DECEL_CONTROL;
    }
    else if (0 == strcasecmp(tp_string, "ModNext"))
    {
      restriction[i].type =  RESTRICT_MOD_NEXT;
    }
    else
    {
      printf("******* Invalid restriction type on line %d: '%s'\n", line_num, tp_string);
      continue;
    }

    if (restriction[i].start <= last_start)
    {
      printf("******* Invalid start point, MUST be in assending order, line %d\n", line_num);
    }  
    if (restriction[i].start >= restriction[i].end)
    {
      printf("******* Invalid, start point MUST be before the end, line %d\n", line_num);
    }
    last_start = restriction[i].start;
    print_restriction(&restriction[i]);

    if (mod_next == true)
    {
      mod_next = false;
      // For now, you can only modify the start, end, and max_speed;
      if (modifier.start > 0.0)
      {
        restriction[i].start = modifier.start;
      }
      if (modifier.end > 0.0)
      {
        restriction[i].end = modifier.end;
      }
      if (modifier.max_speed > 0.0)
      {
        restriction[i].max_speed = modifier.max_speed;
      }
    }

    if (restriction[i].type == RESTRICT_MOD_NEXT)
    {
      // If this modifies the next restriction, save it and continue;
      mod_next = true;
      modifier = restriction[i];
    }
    else
    {
      i++;
      n_restrictions++;
    }
  }
  return n_restrictions;
}


/*********************************************************************/

