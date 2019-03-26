

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "rt_json.h"

/***********************************************/

const char *rt_json_type_string(int tp)
{
  switch (tp)
  {
    case RT_JSON_EMPTY: return "RT_JSON_EMPTY";
    case RT_JSON_NULL: return "RT_JSON_NULL";
    case RT_JSON_NUMBER: return "RT_JSON_NUMBER";
    case RT_JSON_BOOL: return "RT_JSON_BOOL";
    case RT_JSON_STRING: return "RT_JSON_STRING";
    case RT_JSON_OBJECT: return "RT_JSON_OBJECT";
    case RT_JSON_ARRAY: return "RT_JSON_ARRAY";
    case RT_JSON_ERROR: return "RT_JSON_ERROR"; 
    default: return "INVALID";
  }
}


/***********************************************/

char *rt_json_skip_number(char *p)
{
  printf("skip number\n");
  while(true)
  {
    if ((isdigit(*p)) || (*p == '-') || (*p == '-')  || (*p == 'e') || (*p == '+') || (*p == '.'))   
    {
      p++;
    }
    else
    {
      break;
    }
  }
  return p;
}

/***********************************************/


char *rt_json_skip_string(char *p)
{
  printf("skip string\n");
  p++;
  while (true)
  {
    if (*p == '"')
    {
      p++;
      return p;
    }
    else if (*p == '\0')
    {
      return p;
    }
    else if (*p == '\\')
    {
      p++;
      if (*p != '\0') p++;
    }
    else
    {
      p++;
    }
  }
}


/***********************************************/

char * rt_json_skip_object_array(char *buf)
{
  char *p = buf;
  char end_found = false;
  int n_lobj = 0;;
  int n_larray = 0;
  printf("skip object array\n");

  while (isspace(*p)) p++;
  if ((*p != '{') && (*p != '[')) return p;

  while (!end_found)
  {
    printf("ch = %c, n_lobj = %d, n_larray = %d\n", *p, n_lobj, n_larray);
    if (*p == '\"') {p = rt_json_skip_string(p);}
    else if (*p == '\0') {return p;} 
    else if (*p == '[') {n_larray ++; p++;} 
    else if (*p == ']') {n_larray--; p++;} 
    else if (*p == '{') {n_lobj++; p++;} 
    else if (*p == '}') {n_lobj--; p++;} 
    else {p++;}

    if ((n_larray == 0) && (n_lobj == 0))
    {
      return p;
    }
  }
  return p; 
  while(true)
  {

  }
}


/***********************************************/

char *rt_json_skip_value(char *buf)
{
  char *p = buf;
  printf("skip value: %s\n", buf);
  while (isspace(*p)) p++;
  while (true)
  {
    if (*p == '\0')
    {
      return p;
    }
    else if ((*p == '-') || isdigit(*p)) 
    {
      return rt_json_skip_number(p); 
    }
    else if (0 == strncmp(p, "null", 4))  
    {
      return p + 4;
    }
    else if (0 == strncmp(p, "true", 4))  
    {
      return p + 4;
    }
    else if (0 == strncmp(p, "false", 5))  
    {
      return p + 5;
    }
    else if (*p == '\"')
    {
      return rt_json_skip_string(p); 
    }
    else if (*p == '{')
    {
      return rt_json_skip_object_array(p);
    }
    else if (*p == '[')
    {
      return rt_json_skip_object_array(p);
    }
    else
    {
      printf("Unexpected character when skipping value: '%c'\n", *p); 
      p++;
    }
  }
}

/***********************************************/

char * rt_json_get_number(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz)
{
  // This assmues that any leading spaces have been removed.
  char *p = buf;
  //bool is_int = true;
  int state = 0;
  bool end_found = false;
  bool error_found = false;
  err[0] = '\0';
  printf("Number, starting buffer: %s\n", p);
  while (!end_found && !error_found)
  {
    switch (state)
    {
      case 0: // start
        printf("State = 0, char = '%c'\n", *p);
        if (*p == '-')
        {
          p++;
        }
        state = 1;
        break;
      case 1: // no leading '-', or after leading '-' 
        printf("State = 1, char = '%c'\n", *p);
        if (*p == '0') // leading zerors ONLY permitted before decimal point, or as only integer digit.
        {
          p++;
          state = 2;
          break;
        }
        else if ((*p >= '1') && (*p <= '9'))
        {
          state = 3;
          break;
        }
        else
        {
          // error, must be at least one digit;
          snprintf(err, esz, "Must be at least one digit at the start of the number"); 
          error_found = true;
          break;
        }
      case 2: // End of integer part
        printf("State = 2, char = '%c'\n", *p);
        if (*p == '.')
        {
          p++;
          //is_int = false;
          state = 4;
          break;
        }
        else if (isdigit(*p))
        {
          // error, leading zero only permited as only digit before '.' or end of number;
          snprintf(err, esz, "Leading zero only permited as the only digit before '.', or end of number"); 
          error_found = true;
          break;
        }
        else 
        {
          state = 6;
          break;
        }
      case 3: // found at least one digit in integer part, looking for more digits.
        printf("State = 3, char = '%c'\n", *p);
        if (isdigit(*p))
        {
          p++;
          break;
        }
        else
        {
          state = 2;
          break;
        }
      case 4: // decimal point found, we now know it is a float, looking for at least one digit after decimal
        printf("State = 4x, char = '%c'\n", *p);
        if (isdigit(*p))
        {
          state = 5;
          p++;
          break;
        }
        else
        {
          //error, must be at least one digit after '.'
          snprintf(err, esz, "Must be at least one digit after the decimal point"); 
          error_found = true;
          break;
        }
      case 5: // at least one digit found after the decimal point, looking for more digits
        printf("State = 5, char = '%c'\n", *p);
        if (isdigit(*p))
        {
          p++;
          break;
        }
        else
        {
          state = 6;
          break;
        }
      case 6: // found integer part, possibly decimal part, now looking for 'e' 'E', or end of number
        printf("State = 6, char = '%c'\n", *p);
        if ((*p == 'E') || (*p == 'e'))
        {
          //is_int = false;
          p++;
          state = 7;
          break;
        }
        else
        {
          // end of number, all ok.
          end_found = true;
          break;
        }
      case 7: // found 'e' 'E', looking for exponent 
        printf("State = 7, char = '%c'\n", *p);
        if ((*p == '+') || (*p == '-'))
        {
          p++;
        } 
        state = 8; 
        break;
      case 8: // found 'e' 'E', and possible '-' or '+', now must be at least one digit 
        printf("State = 8, char = '%c'\n", *p);
        if (isdigit(*p))
        {
          state = 9; // Found at least one digit!
          p++;
          break;
        }
        else
        {
          //error, must be at least one digit after 'e' or 'E'
          snprintf(err, esz, "Must be at least one digit after 'e' or 'E'"); 
          error_found = true;
          break;
        }
      case 9: // found 'e' 'E', and possible '-' or '+', AND at least one digit 
        printf("State = 9, char = '%c'\n", *p);
        if (isdigit(*p))
        {
          p++;
          break;
        }
        else
        {
          // End of number, all ok.
          end_found = true;
        }
    }
  }
  printf("Number, ending buffer: '%s'\n", buf);
  if (!error_found)
  {
    double number = atof(buf);
    printf("Number = %lg\n", number);
    *type = RT_JSON_NUMBER;
    argv[0] = buf;
    *actual_argc = 0;
    return p;
  }
  else
  {
    printf("Error = '%s'\n", err);
    *type = RT_JSON_ERROR;
    *actual_argc = 0;
    return p;
  }
}

/***********************************************/

char *rt_json_skip_colon(char *p, bool *found)
{
  bool found_colon = false;
  while (true)
  {
    if ((*p == ':') && (!found_colon))
    {
      found_colon = true;
      p++;
    }
    else if (isspace(*p))
    {
      p++;
    }
    else
    {
      *found = found_colon;
      return p;
    }
  }
}

/***********************************************/

char *rt_json_find_spaces_one_comma(char *p, bool *found)
{
  printf("Destrying spaces, comma\n");
  bool found_comma = false;
  while (true)
  {
    if ((*p == ',') && (!found_comma))
    {
      found_comma = true;
      *p = '\0';
      p++;
    }
    else if (isspace(*p))
    {
      *p = '\0';
      p++;
    }
    else
    {
      *found = found_comma;
      return p;
    }
  }
}
/***********************************************/

char *rt_json_get_object(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz)
{
  char *p = buf;
  char *next;
  int n_fields = 0; 
  int n;
  bool found;
  printf("getting object . . . \n");
  if (*p != '{')
  {
    *type = RT_JSON_ERROR;
    snprintf(err, esz, "Objects must start with '{'");
    return p; 
  }
  p++; // skip the leading '{' 

  while (true)
  {
    while (isspace(*p)) p++; // first skip any leading spaces.
    if (*p == '}')
    {
      *p = '\0';
      *actual_argc = n_fields;
      *type = RT_JSON_OBJECT;
      err[0] = '\0';
      return p+1; 
    }
    if (*p == '\"') 
    {
      argv[n_fields] = p;
      p = rt_json_skip_string(p);
    }
    else 
    {
      *type = RT_JSON_ERROR;
      *actual_argc = n_fields;
      snprintf(err, esz, "Objects elements must start with '\"'");
      return p; 
    }
    p = rt_json_skip_colon(p, &found);
    if (!found)
    {
      *type = RT_JSON_ERROR;
      *actual_argc = n_fields;
      snprintf(err, esz, "Objects elements must have ':' after string");
      return p; 
    }
    p = rt_json_skip_value(p); 
    n_fields++;
    p = rt_json_find_spaces_one_comma(p, &found);
    if ((!found) && (*p != '}'))
    {
      *type = RT_JSON_ERROR;
      *actual_argc = n_fields;
      snprintf(err, esz, "Objects elements must have ',' before next value");
      return p; 
    }
  }
  char *string_argv[1];
  next =  rt_json_get_string(p, string_argv, 1, &n, type, err, esz);
  *type = RT_JSON_OBJECT;
  *actual_argc = 0;
  err[0] = '\0';
  return next;
}

/***********************************************/

char *rt_json_get_array(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz)
{
  char *p = buf;
  char *next;
  int n_fields = 0; 
  int n;
  bool found;
  printf("getting array . . . \n");
  if (*p != '[')
  {
    *type = RT_JSON_ERROR;
    snprintf(err, esz, "Objects must start with '{'");
    return p; 
  }
  p++; // skip the leading '[' 

  while (true)
  {
    while (isspace(*p)) p++; // first skip any leading spaces.
    argv[n_fields] = p;
    if (*p == ']')
    {
      *p = '\0';
      *actual_argc = n_fields;
      *type = RT_JSON_ARRAY;
      err[0] = '\0';
      return p+1; 
    }

    p = rt_json_skip_value(p); 
    n_fields++;
    p = rt_json_find_spaces_one_comma(p, &found);
    if ((!found) && (*p != ']'))
    {
      *type = RT_JSON_ERROR;
      *actual_argc = n_fields;
      snprintf(err, esz, "Objects elements must have ',' before next value");
      return p; 
    }
    printf("Rest = '%s'\n", p);
  }
  char *string_argv[1];
  next =  rt_json_get_string(p, string_argv, 1, &n, type, err, esz);
  *type = RT_JSON_ARRAY;
  *actual_argc = 0;
  err[0] = '\0';
  return next;
}


/***********************************************/

char *rt_json_get_string(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz)
{
  char *p = buf;
  if (*p != '"')
  {
    *type = RT_JSON_ERROR;
    snprintf(err, esz, "Strings must start with '\"'");
    *actual_argc = 0;
    return p; 
  }
  p++; // skip the leading quote
  bool end_found = false;
  printf("Starting loop to check string\n");
  while (!end_found)
  {
    if (*p == '\0') 
    {
      printf("Early end of string\n");
      *type = RT_JSON_ERROR;
      snprintf(err, esz, "Unexpected end of string");
      *actual_argc = 0;
      return p; 
    }
    else if (*p == '\\') 
    {
      printf("found escape character\n");
      p++;
      if (p == '\0') // There must be on character after the excape symbol
      {
        printf("Early end of string after escape character\n");
        *type = RT_JSON_ERROR;
        snprintf(err, esz, "Unexpected end of string");
        *actual_argc = 0;
        return p; 
      }
      p++;
    } 
    else if (*p == '"')
    {
      printf("Ending quote found\n");
      *p = '\0';
      p++;
      end_found = true;
    }
    else
    {
      printf("normal char = '%c'\n", *p);
      p++;
    }
  }
  *type = RT_JSON_STRING;
  err[0] = '\0';
  argv[0] = buf + 1; 
  printf("String = '%s'\n", argv[0]);
  *actual_argc = 1;
  return p;
}

/***********************************************/

char *rt_json_next_value(char *buf, char *argv[], int max_argc, int *actual_argc, int *type, char *err, int esz)
{
  char *p = buf;
  err[0] = '\0';

  printf("starting buffer: '%s'\n", buf);
  while (isspace(*p) && (*p != '\0')) p++;
  printf("First char: '%c'\n", *p);
  if (*p == '\0')
  {
    *type = RT_JSON_EMPTY;
    err[0] = '\0';
    *actual_argc = 0;
    return p;
  }
  else if ((*p == '-') || isdigit(*p)) 
  {
    return rt_json_get_number(p, argv, max_argc, actual_argc, type, err, esz); 
  }
  else if (0 == strncmp(p, "null", 4))  
  {
    //*(p + 4) = '\0';
    *type = RT_JSON_NULL;
    argv[0] = p;
    err[0] = '\0';
    *actual_argc = 1;
    return p + 4;
  }
  else if (0 == strncmp(p, "true", 4))  
  {
    //*(p + 4) = '\0';
    *type = RT_JSON_BOOL;
    argv[0] = p;
    err[0] = '\0';
    *actual_argc = 1;
    return p + 1;
  }
  else if (0 == strncmp(p, "false", 5))  
  {
    //*(p + 5) = '\0';
    *type = RT_JSON_BOOL;
    argv[0] = p;
    err[0] = '\0';
    *actual_argc = 1;
    return p + 1;
  }
  else if (*p == '\"')
  {
    return rt_json_get_string(p, argv, max_argc, actual_argc, type, err, esz); 
  }
  else if (*p == '{')
  {
    return rt_json_get_object(p, argv, max_argc, actual_argc, type, err, esz); 
  }
  else if (*p == '[')
  {
    return rt_json_get_array(p, argv, max_argc, actual_argc, type, err, esz); 
  }

  snprintf(err, esz, "Unexpected character: '%c', at position %d\n", *p, p - buf);  
  *type =  RT_JSON_ERROR;
  *actual_argc = 0;
  return p;

}  

/***********************************************/

void rt_json_print_result(int type, int xargc, char *xargv[]) 
{
  printf("type = %s\n", rt_json_type_string(type));
  for (int i=0; i < xargc; i++)
  {
    printf("%d: '%s'\n", i, xargv[i]);
  }
  printf("-------\n");
}

/***********************************************

int main(int argc, char *argv[])
{
  int xargc;
  int type;
  char *xargv[200];
  char *next;
  char err[200];
  char buf[200];
  snprintf(buf, sizeof(buf), "    true    ");

  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);

  snprintf(buf, sizeof(buf), "    null    ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);

  snprintf(buf, sizeof(buf), "    false    ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "     ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);

  snprintf(buf, sizeof(buf), "  \" \"   ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  {test:\"what?\" }   ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  -12.35e56   ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  -012.35e56   ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  -12.e12   ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  1.  ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);

  snprintf(buf, sizeof(buf), "  1.1e  ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  -101e+12  ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), "  \"this is a string!!\\n\\r\\t\"    ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  rt_json_print_result(type, xargc, xargv);
  
  snprintf(buf, sizeof(buf), " {\"test\":[[3,5],{\" \\\"test\":null}], \"what?\": 10.2 , \"where\":false  } end ");
  next = rt_json_skip_value(buf); 
  printf("Rest of string: '%s'\n", next);
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  if (type == RT_JSON_ERROR) printf("ERROR: %s\n", err);
  rt_json_print_result(type, xargc, xargv);

  snprintf(buf, sizeof(buf), " [[6,7], 8, {\"test\":false, \"which\", [3,4]}, [3,5], 7] end ");
  next = rt_json_next_value(buf, xargv, sizeof(argv)/sizeof(argv[0]), &xargc, &type, err, sizeof(err));
  printf("Rest of string: '%s'\n", next);
  if (type == RT_JSON_ERROR) printf("ERROR: %s\n", err);
  rt_json_print_result(type, xargc, xargv);
}

***********************************************/

