

#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
  time_t now;
  struct tm mytm;
  char buf[100];

  now = time(NULL);
  localtime_r(&now, &mytm);
  printf("%s\n", asctime(&mytm));

  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", &mytm);
  printf("%s\n",  buf);

  strptime(buf, "%Y-%m-%dT%H:%M:%S%z", &mytm);
  printf("%s\n\n\n", asctime(&mytm));

  strptime("2010-08-29T14:12:10-0200", "%Y-%m-%dT%H:%M:%S%z", &mytm);
  printf("%s\n", asctime(&mytm));

  strptime("2010-08-29T14:12:10+0500", "%Y-%m-%dT%H:%M:%S%z", &mytm);
  printf("%s\n", asctime(&mytm));

}
