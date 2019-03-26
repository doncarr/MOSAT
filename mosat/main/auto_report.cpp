#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

void print_help()
{
  printf("Usage: auto_report [-d <home-directory>]\n");
  printf("  -d   use an alternate directory for the home directory.\n");
  printf("       The -d command must be followed by a home directory.\n");
  printf("       This will be used as the home directory, in place of\n");
  printf("       the current directory.\n\n");
}

int main (int argc,char *argv[])
{
  int current_arg;
  const char *home_dir="./";
  for (current_arg=1; current_arg < argc; current_arg++)
  {
    if (0 == strcmp(argv[current_arg], "-d"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        home_dir = argv[current_arg];
        chdir(home_dir);
        printf("home dir = %s\n",home_dir);
        home_dir = "./";
      }
      else
      {
        print_help();
        printf("For -d option, you MUST specify the directory, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "--help"))
    {
      print_help();
      exit(0);
    }
  }
  system("pwd");
  while(1)
  {
    system("../main/frames 7200");
    sleep(30);
  }
}
