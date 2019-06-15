
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "xjson.h"


/**************************************************************/

void do_xjson_print_indented(xjson_parse_t *xjp, int indent, int *n, int is_key, int do_indent)
{
  if (*n > xjp->current_value)
  {
    return;
  }
  if (do_indent) for (int i=0; i < indent; i++) printf("  ");
  xjson_value_t *vv = &(xjp->values[*n]);
  switch (vv->type)
  {
    case XJSON_OBJECT:
      printf ("Object, %d fields:\n", vv->count);
      for (int i = 0; i < vv->count; i++)
      {
        (*n)++;
        do_xjson_print_indented(xjp, indent + 1, n, 1, 1);
        (*n)++;
        xjson_value_t *vnext = &(xjp->values[(*n)]);
        if ((vnext->type == XJSON_ARRAY) || (vnext->type == XJSON_OBJECT))
        {
          printf("\n");
          do_xjson_print_indented(xjp, indent + 2, n, 0, 1);
        }
        else
        {
          do_xjson_print_indented(xjp, indent + 2, n, 0, 0);
        }
      }
      break;
    case XJSON_ARRAY:
      printf ("Array, %d elements:\n", vv->count);
      for (int i = 0; i < vv->count; i++)
      {
        (*n)++;
        do_xjson_print_indented(xjp, indent + 1, n, 0, 1);
      }
      break;
    case XJSON_STRING:
      printf("\"%.*s\"", vv->n_chars, vv->start);
      if (is_key) printf(":");
      else printf("\n");
      break;
    case XJSON_NUMBER:
    case XJSON_TRUE:
    case XJSON_FALSE:
    case XJSON_NULL:
      printf("%.*s\n", vv->n_chars, vv->start);
      break;
    case XJSON_UNDEFINED:
      printf("Undefined");
      break;
    default:
      printf("******");
      break;
  }
}

/**************************************************************/

void xjson_print_indented(xjson_parse_t *xjp)
{
  int n = 0;
  do_xjson_print_indented(xjp, 0, &n, 0, 1);
  printf("%d values printed\n", xjp->current_value);
}

/**************************************************************/

int main(int argc, char *argv[])
{
  size_t json_size;
  char *jj;
  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL)
  {
    perror(argv[1]);
    return 1;
  }

  struct rlimit myrlimit;

  getrlimit(RLIMIT_STACK, &myrlimit);
  printf("soft: %ld\n", myrlimit.rlim_cur);
  printf("hard: %ld\n", myrlimit.rlim_max);

  myrlimit.rlim_cur *= 4;
  setrlimit(RLIMIT_STACK, &myrlimit);

  getrlimit(RLIMIT_STACK, &myrlimit);
  printf("soft: %ld\n", myrlimit.rlim_cur);
  printf("hard: %ld\n", myrlimit.rlim_max);

  fseek(fp, 0, SEEK_END);
  json_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  printf("allocating: %zu bytes\n", json_size);
  jj = malloc(json_size+1);
  if (jj == NULL)
  {
    perror(argv[1]);
    return 1;
  }
  fread(jj, json_size, 1, fp);
  jj[json_size] = '\0';

/**
int clock_gettime(clockid_t clk_id, struct timespec *tp);
CLOCK_MONOTONIC_
struct timespec
{
  time_t   tv_sec;        // seconds
  long     tv_nsec;       // nanoseconds
};
***/



  //printf("time: %ju\n", time(NULL));
  printf("json length: %zu\n", strlen(jj));
  //printf("json: %s\n", jj);

  xjson_parse_t the_parser;
  xjson_init(&the_parser, jj);

  struct timespec start_time;
  struct timespec end_time;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
  if (xjson_get_value(&the_parser) < 0)
  {
    printf("Error at position %d, line %d, col %d: found this: %.10s\n",
      the_parser.error_offset, the_parser.line_number,
      the_parser.error_offset - the_parser.newline_position,
      the_parser.json_string + the_parser.error_offset);
    printf("%s\n", the_parser.error_string);
    return 1;
  }
  // Get the end time right after parsing.
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);

  printf("%d values found\n", the_parser.current_value + 1);
  /**
  printf("================================\n");
  xjson_value_t *vv;
  for (int i=0; i <= the_parser.current_value; i++)
  {
    vv = &the_parser.values[i];
    xjson_print_value(vv);
  }
  **/
  printf("================================\n");
  xjson_print_indented(&the_parser);
  //xjson_free(&the_parser);
 
  if ((end_time.tv_nsec - start_time.tv_nsec) < 0)
  {
    end_time.tv_nsec += 1000000000;
    end_time.tv_sec -= 1;
  }
  double parse_time = ((double)((end_time.tv_sec - start_time.tv_sec) * 1000)) + 
                      (((double)(end_time.tv_nsec - start_time.tv_nsec))/1000000.0);

  printf("It took %0.3lf milliseconds to parse %lf megabytes\n",
      parse_time, ((double)(json_size)) / 1000000.0);
  return 0;
}

/**************************************************************/


