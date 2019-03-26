
#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("Hello World\n");
#ifdef __cplusplus
printf("Wow, C++\n");
#else
printf("Boooring, only C\n");
#endif
printf("Hey!! this thing works!!\n");
  return 0;
}
