
#ifndef __xjson_INC__
#define __xjson_INC__

typedef enum
{
  XJSON_UNDEFINED = 0,
  XJSON_OBJECT = 1,
  XJSON_ARRAY = 2,
  XJSON_STRING = 3,
  XJSON_NUMBER = 4,
  XJSON_TRUE = 5,
  XJSON_FALSE = 6,
  XJSON_NULL = 7,
} xjson_value_type_t;

typedef enum
{
  XJSON_OK = 0,
  XJSON_INVALID_START = 1,
  XJSON_END_OF_STRING = 2,
  XJSON_REALLOC_FAIL = 3,
  XJSON_EXPECT_COMMA_BRACKET = 4,
  XJSON_EXPECT_COLON = 5,
  XJSON_EXPECT_KEY_STRING = 6,
  XJSON_EXPECT_NULL = 7,
  XJSON_EXPECT_FALSE = 8,
  XJSON_EXPECT_TRUE = 9,
  XJSON_INVALID_AFTER_NUMBER = 10,
  XJSON_EXPECT_DIGIT_AFTER_E = 11,
  XJSON_EXPECT_DIGIT_AFTER_DECIMAL = 12,
  XJSON_DIGIT_AFTER_LEAD_ZERO = 13,
  XJSON_NO_END_QUOTE = 14,
  XJSON_ESCAPE_CONTROL = 15,
  XJSON_INVALID_AFTER_ESCAPE = 16,
  XJSON_FOUR_DIGITS_AFTER_U = 17,
  XJSON_MALLOC_FAIL = 18,
  XJSON_EXPECT_COMMA_CURLY

}xjson_error_t;

typedef struct xjson_value
{
  xjson_value_type_t type;
  char *start;
  int n_chars;
  int count;
  struct xjson_value *parent;
} xjson_value_t;

typedef struct
{
  xjson_value_t *values;
  int max_values;
  int current_value;
  char *json_string;
  char *json_position;
  xjson_error_t error;
  char error_string[100];
  int error_offset;
  int line_number;
  int newline_position;
} xjson_parse_t;

#define false 0
#define true 1

// We could group these under "strict" or non "strict"?
#define ALLOW_OBJECT_TRAILING_COMMA (1)
#define ALLOW_ARRAY_TRAILING_COMMA (1)

int xjson_get_value(xjson_parse_t *xjp);
void xjson_print_value(xjson_value_t *vv);
const char *xjson_get_type_string(xjson_value_type_t tt);
int xjson_reset(xjson_parse_t *xjp, char *the_json);
int xjson_init(xjson_parse_t *xjp, char *the_json);
void xjson_free(xjson_parse_t *xjp);

#endif
