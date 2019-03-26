

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  FILE *fp1= fopen("xx.txt", "r");
  if (fp1 == NULL)
  {
    printf("can't open 'xx.txt'\n");
    exit(1);
  }
  FILE *fp2 = fopen("out_xx.txt", "w");
  if (fp2 == NULL)
  {
    printf("can't open 'out_xx.txt'\n");
    exit(1);
  }
  FILE *fp3 = fopen("fxx.txt", "r");
  if (fp3 == NULL)
  {
    printf("can't open 'fxx.txt'\n");
    exit(1);
  }

  char line[300];
  float tmp1, hum1, fan_val;
  time_t then;
  time_t fan_time;
  //char tagt[200];
  //char tagh[200];
  struct tm mytm;
  bool fan_on = false;
  bool next_fan = false;
  bool fan_end = false;


//temp_amb,14,1259832600
//hum_rel,93.8,1259832600

  char *p;

  fgets(line, sizeof(line), fp3);
  for(p=line; (*p != ',') && (*p != '\0'); p++){};
  p++;
  next_fan = (*p != '0');
  printf("---------- next_fan: %c: %s\n", *p, next_fan ? "true":"false");
  printf("%s", p);
  for( ; (*p != ',') && (*p != '\0'); p++){};
  p++;
  fan_time = atol(p);
  //fan_val = fan_on ? 78.0 : 72.0;
  fan_val = fan_on ? 59.0 : 51.0;

  int count=0;
  for (int i=0; NULL != fgets(line, sizeof(line), fp1); i++)
  {
    for(p=line; (*p != ',') && (*p != '\0'); p++){};
    p++;
    tmp1 = atof(p);
    printf("%s", p);
    fgets(line, sizeof(line), fp1);
    for(p=line; (*p != ',') && (*p != '\0'); p++){};
    p++;
    hum1 = atof(p);
    printf("%s", p);
    for( ; (*p != ',') && (*p != '\0'); p++){};
    p++;
    then = atol(p);
    printf("%s", p);

    localtime_r(&then, &mytm);
    printf("then: %s\n", asctime(&mytm));

    if ((then >= fan_time) && !fan_end)
    {
      fan_on = next_fan;
      if (NULL != fgets(line, sizeof(line), fp3))
      {
        for(p=line; (*p != ',') && (*p != '\0'); p++){};
        p++;
        next_fan = (*p != '0');
        printf("---------- next_fan: %c: %s\n", *p, next_fan ? "true":"false");
        printf("%s", p);
        for( ; (*p != ',') && (*p != '\0'); p++){};
        p++;
        fan_time = atol(p);
      }
      else
      {
        fan_end = true;
        printf("########################### End of fan file\n");
      }
    }

    //fan_val = fan_on ? 78.0 : 72.0;
    fan_val = fan_on ? 59.0 : 51.0;

    fprintf(fp2, "%d\t%0.1f\t%0.1f\t%0.1f\n", count, tmp1, hum1, fan_val);
    printf("%d\t%0.1f\t%0.1f\t%0.1f\n", count, tmp1, hum1, fan_val);
    count++;
  }

}
