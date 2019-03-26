
#ifndef __RT_JSON_INC__
#define __RT_JSON_INC__

#define RT_JSON_EMPTY (1)
#define RT_JSON_NULL (2)
#define RT_JSON_NUMBER (3)
#define RT_JSON_BOOL (4)
#define RT_JSON_STRING (5)
#define RT_JSON_OBJECT (6)
#define RT_JSON_ARRAY (7)
#define RT_JSON_ERROR (-1)

const char *rt_json_type_string(int tp);
char * rt_json_get_string(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz);
char *rt_json_skip_number(char *p);
void rt_json_print_result(int type, int xargc, char *xargv[]);
char *rt_json_next_value(char *buf, char *argv[], int max_argc, int *actual_argc, int *type, char *err, int esz);
char *rt_json_get_string(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz);
char *rt_json_get_array(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz);
char *rt_json_get_object(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz);
char *rt_json_skip_colon(char *p, bool *found);
char *rt_json_find_spaces_one_comma(char *p, bool *found);
char *rt_json_skip_value(char *buf);
char * rt_json_get_number(char *buf, char *argv[], int max_argv, int *actual_argc, int *type, char *err, int esz);
char *rt_json_skip_string(char *p);
char *rt_json_skip_object_array(char *buf);

#endif
