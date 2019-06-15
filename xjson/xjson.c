
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

const char *xjson_get_type_string(xjson_value_type_t tt)
{
    switch (tt)
    {
      case XJSON_OBJECT:
        return("Object");
        break;
      case XJSON_ARRAY:
        return("Array");
        break;
      case XJSON_STRING:
        return("String");
        break;
      case XJSON_NUMBER:
        return("Number");
        break;
      case XJSON_TRUE:
        return("True");
        break;
      case XJSON_FALSE:
        return("False");
        break;
      case XJSON_NULL:
        return("Null");
        break;
      case XJSON_UNDEFINED:
        return("Undefined");
        break;
      default:
        return("******");
        break;
    }
  return "";
}

/**************************************************************/

void xjson_print_value(xjson_value_t *vv)
{
  printf("---------------------\n");
  //printf("pointer: %p, ", vv);
  printf("%s: ", xjson_get_type_string(vv->type));
  if ((vv->type == XJSON_ARRAY) || (vv->type == XJSON_OBJECT))
  {
    printf("count = %d, ", vv->count);
  }
  printf("n_char = %d\n", vv->n_chars);
  printf("str = |%.*s|\n", vv->n_chars, vv->start) ;
}

/**************************************************************/

int xjson_get_value(xjson_parse_t *xjp);

/**************************************************************/

char xjson_next_char(xjson_parse_t *xjp)
{
  // return the next character, without advancing.
  //char next = *(xjp->json_position);
  //printf("Next char: %c\n", next);
  //return (next);
  return *(xjp->json_position);
}

/**************************************************************/

void xjson_skip_char(xjson_parse_t *xjp)
{
  // Skip just one character before end of string.
  if ((*(xjp->json_position)) != '\0')
  {
    // Really, it should be an internal error if we try to
    // advance past the string terminator.
    // So, we could put a check here during testing to verify
    // that we never call skip at the end of the string.
    xjp->json_position++;
  }
}

/**************************************************************/

void xjson_skip_space(xjson_parse_t *xjp)
{
  // Skip all white space.
  while (isspace(*(xjp->json_position)))
  {
    if (*(xjp->json_position) == '\n')
    {
      xjp->line_number++;
      xjp->newline_position = (xjp->json_position - xjp->json_string);
    }
    xjp->json_position++;
  }
  //printf("skip space, next char: %c\n", *(xjp->json_position));
}

/**************************************************************/

int xjson_get_string(xjson_parse_t *xjp)
{
  xjson_value_t *vv = &(xjp->values[xjp->current_value]);
  vv->type = XJSON_STRING;
  vv->count = 0;
  xjp->json_position++;
  vv->start = xjp->json_position;
  //printf("vv->start: %s\n", vv->start);
  char *pp = vv->start;
  // Search for the end of the string.
  while((*pp != '"') && (*pp != '\0'))
  {
    if (*pp == '\\')
    {
      pp++;
      // See http://json.org for a listing of valid characters after a '\'
      switch (*pp)
      {
        case '"':
        case '\\':
        case '/':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
          pp++;
          break;
        case 'u':
          pp++;
          for (int i=0; i<4; i++)
          {
            if (!isxdigit(pp[i]))
            {
              xjp->json_position = pp;
              snprintf(xjp->error_string, sizeof(xjp->error_string),
                            "Must have 4 hex digits after '\\u' in a string, invalid: '%c'", pp[i]);
              xjp->error_offset = (xjp->json_position - xjp->json_string);
              return -1;
            }
          }
          pp += 4;
          break;
        default:
          // Should we be this strict about what can appear after a '\'??
          xjp->json_position = pp;
          snprintf(xjp->error_string, sizeof(xjp->error_string), "Invalid character after '\\' in a string: %c", *pp);
          xjp->error_offset = (xjp->json_position - xjp->json_string);
          return -1;
          break;
      }
    }
    else
    {
      //
      // Should we allow actual \n, \r, \t, \f, \b in the string??
      //
      // Should we really allow almost anything in a string?
      pp++;
    }
  }
  if (*pp == '\0')
  {
    vv->type = XJSON_UNDEFINED;
    xjp->json_position = pp;
    snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "No ending quote for string");
    xjp->error_offset = (xjp->json_position - xjp->json_string);
    return -1;
  }
  pp++;
  xjp->json_position = pp;
  vv->n_chars = pp - vv->start-1;
  return 0;
}


/**************************************************************/

int xjson_get_number(xjson_parse_t *xjp)
{
  // See http://json.org for a listing of valid numbers.
  // For instance these are allowed: 1e-0 1e+0 1e0
  // These are NOT allowed: 1. 1.e3 00 01 02
  xjson_value_t *vv = &(xjp->values[xjp->current_value]);
  vv->type = XJSON_NUMBER;
  vv->count = 0;
  vv->start = xjp->json_position;
  //printf("vv->start: %s\n", vv->start);
  char *pp = vv->start;
  if (*pp == '-')
  {
    pp++;
  }

  //printf("1: %.20s\n", pp);
  if (*pp == '0')
  {
    pp++;
    if (isdigit(*pp))
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "leading 0 followed by digit now allowed");
      xjp->json_position = pp;
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
    }
  }
  else
  {
    while(isdigit(*pp))
    {
      pp++;
    }
  }
  //printf("2: %.20s\n", pp);

  if (*pp == '.')
  {
    pp++;
    if (!isdigit(*pp))
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Must have at leaset one digit after decimal point");
      xjp->json_position = pp;
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
    }
    while(isdigit(*pp))
    {
      pp++;
    }
  }
  //printf("3: %.20s\n", pp);

  if ((*pp == 'e') || (*pp == 'E'))
  {
    pp++;
    //printf("5: %.20s\n", pp);
    if ((*pp == '+') || (*pp == '-'))
    {
      pp++;
    }
    //printf("6: %.20s\n", pp);
    if (!isdigit(*pp))
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Must have at leaset one digit after 'e' or 'E'");
      xjp->json_position = pp;
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
    }
    while(isdigit(*pp))
    {
      pp++;
    }
  }
  //printf("7: %.20s\n", pp);
  switch (*pp)
  {
    case ',':
    case ':':
    case '}':
    case ']':
    case '\0':
      break;
    default:
      if (!isspace(*pp))
      {
        snprintf(xjp->error_string, sizeof(xjp->error_string), "Invalid character in number: '%c'", *pp);
        xjp->json_position = pp;
        xjp->error_offset = (xjp->json_position - xjp->json_string);
        return -1;
      }
      break;
  }
  vv->n_chars = pp - vv->start;
  xjp->json_position = pp;
  return 0;
}

/**************************************************************/

int xjson_get_bool(xjson_parse_t *xjp)
{
  xjson_value_t *vv = &(xjp->values[xjp->current_value]);
  vv->start =  xjp->json_position;
  //printf("vv->start: %s\n", vv->start);
  if (0 == strncmp(vv->start, "true", 4))
  {
    vv->type = XJSON_TRUE;
    vv->count = 0;
    vv->n_chars = 4;
    xjp->json_position += 4;
  }
  else if (0 == strncmp(vv->start, "false", 5))
  {
    vv->type = XJSON_FALSE;
    vv->count = 0;
    vv->n_chars = 5;
    xjp->json_position += 5;
  }
  else
  {
    vv->type = XJSON_UNDEFINED;
    vv->count = 0;
    vv->n_chars = 0;
    snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Expected 'true' or 'false'");
    xjp->error_offset = (xjp->json_position - xjp->json_string);
    return -1;
  }
  return 0;
}

/**************************************************************/

int xjson_get_null(xjson_parse_t *xjp)
{
  xjson_value_t *vv = &(xjp->values[xjp->current_value]);
  vv->start = xjp->json_position;
  //printf("vv->start: %s\n", vv->start);
  if (0 == strncmp(vv->start, "null", 4))
  {
    vv->type = XJSON_NULL;
    vv->count = 0;
    vv->n_chars = 4;
    xjp->json_position += 4;
  }
  else
  {
    vv->type = XJSON_UNDEFINED;
    vv->count = 0;
    vv->n_chars = 0;
    snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Expected 'null'\n");
    xjp->error_offset = (xjp->json_position - xjp->json_string);
    return -1;
  }
  return 0;
}

/**************************************************************/

int xjson_get_object(xjson_parse_t *xjp)
{
  int done = false;
  xjson_value_t *vv = &(xjp->values[xjp->current_value]);
  vv->start =  xjp->json_position;
  //printf("vv->start: %s\n", vv->start);
  vv->type = XJSON_OBJECT;
  vv->count = 0;
  xjson_skip_char(xjp);
  xjson_skip_space(xjp);
  if ((xjson_next_char(xjp)) == '}')
  {
    // Empty object is OK
    xjson_skip_char(xjp);
    vv->n_chars = xjp->json_position - vv->start;
    return 0;
  }
  while (!done)
  {
    xjp->current_value += 1;
    if (xjson_get_value(xjp) < 0) return -1;
    xjson_value_t *key = &(xjp->values[xjp->current_value]);
    if (key->type != XJSON_STRING)
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "Key must be a String, but is of type %s\n", xjson_get_type_string(key->type));
      xjp->error_offset = (vv->start - xjp->json_string);
      return -1;
    }
    xjson_skip_space(xjp);
    if ((xjson_next_char(xjp)) == ':')
    {
      xjson_skip_char(xjp);
    }
    else
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Expected ':'");
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
    }
    xjp->current_value += 1;
    if (xjson_get_value(xjp) < 0) return -1;
    xjson_skip_space(xjp);
    if ((xjson_next_char(xjp)) == ',')
    {
      xjson_skip_char(xjp);
      vv->count++;
      xjson_skip_space(xjp);
      if (((xjson_next_char(xjp)) == '}') && ALLOW_OBJECT_TRAILING_COMMA)
      {
        // Here we allow a trailing comma with not value present.
        xjson_skip_char(xjp);
        break;
      }
      continue;
    }
    else if ((xjson_next_char(xjp)) == '}')
    {
      vv->count++;
      xjson_skip_char(xjp);
      break;
    }
    else
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Expected ',' or '}'");
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
    }
  }
  vv->n_chars = xjp->json_position - vv->start;
  return 0;
}

/**************************************************************/

int xjson_get_array(xjson_parse_t *xjp)
{
  xjson_value_t *vv = &xjp->values[xjp->current_value];
  vv->start =  xjp->json_position;
  //printf("vv->start: %s\n", vv->start);
  vv->type = XJSON_ARRAY;
  vv->count = 0;
  vv->n_chars = 0;
  //printf("address of value struct for array: %p\n", vv);
  xjson_skip_char(xjp); // Skip the  ','
  xjson_skip_space(xjp);
  if (xjson_next_char(xjp) == ']')
  {
    // Empty array is OK, Skip the ']'
    xjson_skip_char(xjp);
    vv->n_chars = xjp->json_position - vv->start;
    return 0;
  }
  int done = false;
  while (!done)
  {
    //printf("vv->count = %d\n", vv->count);
    xjp->current_value += 1;
    if (xjson_get_value(xjp) < 0) return -1;
    xjson_skip_space(xjp);
    if (xjson_next_char(xjp) == ',')
    {
      //printf("   - array found comma\n");
      xjson_skip_char(xjp); // Skip the  ','
      vv->count++;
      xjson_skip_space(xjp);
      if ( (xjson_next_char(xjp) == ']') && ALLOW_ARRAY_TRAILING_COMMA)
      {
        //printf("   - array trailing comma, skipping\n");
        xjson_skip_char(xjp);
        done = true;
        break;
      }
    }
    else if ((xjson_next_char(xjp)) == ']')
    {
      vv->count++;
     // printf("   - array end foound ]\n");
      // Skip the ']'
      xjson_skip_char(xjp);
      done = true;
    }
    else
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Expected ',' or ']'");
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      //printf("Array returning -1\n");
      return -1;
    }
  }
  vv->n_chars = xjp->json_position - vv->start;
  return 0;
}

/**************************************************************/

int xjson_get_value(xjson_parse_t *xjp)
{
  xjson_value_t *vv;
  if (xjp->current_value >= xjp->max_values)
  {
    xjp->max_values *= 2;        
    xjp->values = (xjson_value_t *) realloc(xjp->values, sizeof(xjson_value_t) * (xjp->max_values + 1));
    printf("realloc %d\n", xjp->max_values);
    if (xjp->values == NULL)
    {
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s", "Maximum values exceeded.\n");
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
    }
  }
  // Skip leading spaces
  xjson_skip_space(xjp);
  //printf("next chars: |%.10s|\n", xjp->json_position);
  switch (xjson_next_char(xjp))
  {
    case '{': // Object
      return xjson_get_object(xjp);
      break;
    case '[': // Array
      return xjson_get_array(xjp);
      break;
    case '"': // String
      return xjson_get_string(xjp);
      break;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
         // Number
      return xjson_get_number(xjp);
      break;
    case 't':
    case 'f':
         // Boolean");
      return xjson_get_bool(xjp);
      break;
    case 'n':
         // Null
      return xjson_get_null(xjp);
      break;
    case '\0':
      vv = &xjp->values[xjp->current_value];
      vv->start =  xjp->json_position;
      vv->type = XJSON_UNDEFINED;
      vv->n_chars = 0;
      snprintf(xjp->error_string, sizeof(xjp->error_string), "%s",
                 "End of string reached looking for value\n");
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
      break;
    default:
      vv = &xjp->values[xjp->current_value];
      vv->start =  xjp->json_position;
      vv->type = XJSON_UNDEFINED;
      vv->n_chars = 0;
      snprintf(xjp->error_string, sizeof(xjp->error_string),
               "Invalid starting character looking for value: '%c'\n", xjson_next_char(xjp));
      xjp->error_offset = (xjp->json_position - xjp->json_string);
      return -1;
      break;
  }
  return 0;
}


/**************************************************************/

int xjson_reset(xjson_parse_t *xjp, char *the_json)
{
  // This is so you can re-use the same structure to parse another JSON string.
  xjp->current_value = 0;
  xjp->json_string = the_json;
  xjp->json_position = the_json;
  xjp->error = XJSON_OK;
  xjp->error_string[0] = '\0';
  xjp->error_offset = 0;
  xjp->line_number = 1; // We start on line 1!
  xjp->newline_position = 0;
  return 0;
}

/**************************************************************/
int xjson_init(xjson_parse_t *xjp, char *the_json)
{
  // This is for the first initialization and will also allocate space for the values.
  //xjp->max_values = 4;
  xjp->max_values = 512000;
  xjp->values = (xjson_value_t *) malloc(sizeof(xjson_value_t) * xjp->max_values);
  if (xjp->values == NULL)
  {
    return -1;
  }
  return xjson_reset(xjp, the_json);
}

/**************************************************************/

void xjson_free(xjson_parse_t *xjp)
{
  xjp->max_values = 0;
  if (xjp->values != NULL)
  {
    free(xjp->values);
    xjp->values = NULL;
  }
  xjp->current_value = 0;
  xjp->json_string = NULL;
  xjp->json_position = NULL;
  xjp->error = XJSON_OK;
  xjp->error_string[0] = '\0';
  xjp->error_offset = 0;
}

/**************************************************************/


