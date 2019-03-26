#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "../include/rt_json.h"
#include "../include/rtcommon.h"

bool read_json(char *json_buffer)
{
  char *argv[100];
  int argc;
  int type;
  char *trailing_chars;
  char err[100];

  //char *rt_json_next_value(char *buf, char *argv[], int max_argc, int *actual_argc, int *type, char *err, int esz)  
  trailing_chars = rt_json_next_value(json_buffer, argv, sizeof(argv)/sizeof(argv[0]), 
                              &argc, &type, err, sizeof(err)/sizeof(err[0]));  
  printf("Type = %s\n", rt_json_type_string(type));
  ltrim(trailing_chars);
  if (strlen(trailing_chars) != 0)
  {
    printf("Unexpected characters at end: %s\n", trailing_chars);
    return false;
  }
  if (type == RT_JSON_ERROR)
  {
    printf("Error parsing JSON: %s\n", err);
    return false;
  }
  else if (type != RT_JSON_ARRAY)
  {
    printf("Error parsing, expected JSON ARRAY, found: %s\n", rt_json_type_string(type));
    return false;
  }

  char *item_argv[2];
  int item_argc;
  int item_type;

  for (int i=0; i < argc; i++)
  {
    trailing_chars = rt_json_next_value(argv[i], item_argv, sizeof(item_argv)/sizeof(item_argv[0]), 
                                                &item_argc, &item_type, err, sizeof(err)/sizeof(err[0]));  

    printf("Type = %s\n", rt_json_type_string(item_type));

    ltrim(trailing_chars);
    if (strlen(trailing_chars) != 0)
    {
      printf("Unexpected characters at end of item: %s\n", trailing_chars);
      return false;
    }

    if (item_type == RT_JSON_ERROR)
    {
      printf("Error parsing JSON: %s\n", err);
      return false;
    }
    else if (item_type != RT_JSON_ARRAY)
    {
      printf("Error parsing, expected JSON ARRAY, found: %s\n", rt_json_type_string(item_type));
      return false;
    }

    if (item_argc != 2)
    {
      printf("Error, must have exactly two arguments for every tag / log type\n");
      return false;
    }
     
    char *tag = item_argv[0];
    char *log_type = item_argv[1];
    ltrim(tag);
    rtrim(tag);
    strip_quotes(tag);
    ltrim(log_type);
    rtrim(log_type);
    strip_quotes(log_type);

    //rt_bool_ref_t *bref;
    //bref = react_get_discrete_ref_fn(tag);
    //if (bref == NULL)
    //{
     // printf("BAD discrete tag: %s\n", tag);
      //continue;
    //}
    printf("================================\n"); 
    printf("1: %s\n2: %s\n", tag, log_type);
    printf("================================\n"); 
    switch (log_type[0])
    {
      case 'R':
      case 'r':
        printf("rising only\n");
        break;
      case 'F':
      case 'f':
        printf("falling only\n");
        break;
      case 'B':
      case 'b':
        printf("both rising and falling\n");
        break;
      default:
        printf("******* Error %s: discrete %d, bad param: '%s' log type must be R, F, or B (rising, falling, or both)\n",
                   tag, i, log_type); 
        printf("defaulting to both rising and falling\n");
        return false;
        break;
    }
  }

  return true;
}
int main(int argc, char *argv[])
{
  char buf[500];
  snprintf(buf, sizeof(buf), "%s", "[ [\"tag\" , \"f\"],,  [\"truck\", \"b\"], [\"car\", \"r\"], [\"firetruck\", \"B\"],[\"cat\", \"F\"], [\"dog\", \"B\"], [\"bird\", \"R\"], [\"chicken\", \"B\"]] ");
  printf("%s\n", buf);
  read_json(buf);
}
