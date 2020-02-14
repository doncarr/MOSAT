
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct cryptid
{
  const char *name;
  const char *identity;
  const char *format;
} cryptid_t;

/*************t********************************************************/
void rtrim(char *str)
{
  /* Trim the space from the right of a string. */
  for (int i = (strlen(str) - 1);
        isspace(str[i]) && (i >= 0); str[i--] = '\0');
}
/*************t********************************************************/



int read_cryptids(FILE *fp, cryptid_t *cc, int max)
{
  char line[300];
  char *p1, *p2;
  int line_number = 0;
  int blank_lines = 0;
  int data_lines = 0;
  int comments = 0;
  while (NULL != fgets(line, sizeof(line) - 1, fp))
  {
    line_number++;
    rtrim(line);
    if (0 == strlen(line))
    { 
      blank_lines++;
      continue;
    }
    if (line[0] == '#')
    { 
      comments++;
      continue;
    }

    p1 = strchr(line, ','); 
    if (p1 == NULL)
    {
      fprintf(stderr, "no comma on line %d: %s\n", line_number, line);
      exit(1);
    }
    p2 = strchr(p1 + 1, ','); 
    if (p2 == NULL)
    {
      fprintf(stderr, "no second comma on line  %d: %s\n", line_number, line);
      exit(1);
    }
    if (data_lines >= max)
    {
      fprintf(stderr, "too many items:  %d, %s\n", data_lines + 1, line);
      exit(1);
    }
    *p1 = '\0';
    p1++;
    *p2 = '\0';
    p2++;
    cc[data_lines].name = strdup(line);
    cc[data_lines].identity = strdup(p1);
    cc[data_lines].format = strdup(p2);
    printf("1: '%s'\n", line);
    printf("2: '%s'\n", p1);
    printf("3: '%s'\n", p2);
    printf("---------------\n");
    data_lines++;
  }
  printf("%d lines read\n", line_number);
  printf("%d items found\n", data_lines);
  printf("%d blank lines\n", blank_lines);
  printf("%d comment lines\n", comments);
  return data_lines;
}


int main(int argc, char *argv[])
{
  cryptid_t cc[20];
  int n = read_cryptids(stdin, cc, 20);
  printf("***************\n");
  printf("%d cryptids found!!\n", n); 
  for(int i=0; i < n; i++)
  {
    printf("name: '%s'\n", cc[i].name);
    printf("identity: '%s'\n", cc[i].identity);
    printf("format: '%s'\n", cc[i].format);
    printf("---------------\n");
  } 
}
