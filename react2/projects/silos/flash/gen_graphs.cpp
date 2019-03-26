
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[])
{
  DIR *dir;
  struct dirent *dent;

  const char *dir_name = "./";

  dir = opendir(dir_name);

  if (dir == NULL)
  {
    perror(dir_name);
    return -1;
  }

  const char *pre = "webev";
  const char *post = ".txt";

  int len_pre = strlen(pre);
  int len_post = strlen(post);

  dent = readdir(dir);
  while (dent != NULL)
  {
    int len = strlen(dent->d_name);
    if ((len >= len_pre) && (len >= len_post))
    {
      //printf("%d %s, %d %s, %d %s\n", len, dent->d_name, len_pre, pre, len_post, post);
      if ( (0 == strncmp(pre, dent->d_name , len_pre)) && (0 == strncmp(post, dent->d_name + len - len_post, len_post))) 
      {
        printf("MATCH!!!!!!!!!!! %s\n", dent->d_name);
      }
    }
    dent = readdir(dir);
  }
  closedir(dir);
}


