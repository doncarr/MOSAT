

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  fd_set rfds;
  int retval;

  FILE *fp = fopen(argv[0], "r");
  int fd = fileno(fp);
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  fcntl(fd, F_SETFL, O_NONBLOCK);
  fseek(fp,0,SEEK_END);
  char line[500];
  while(1)
  {
    if (NULL !=  fgets(line, sizeof(line), fp))
    {
      printf("%s", line);
    }
    else
    {
       printf("Waiting IO . . .  \n");
       retval = select(1, &rfds, NULL, NULL, NULL);
       printf("IO available\n");
    }
  }

}
