#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
int main (void)
{
  FILE *fp;
  char date[15];
  char fname[20]="vlog";
  struct tm mytm;
  time_t my_time_t;
  my_time_t = time(NULL);
  localtime_r(&my_time_t,&mytm);
  strftime(date,sizeof(date),"%Y%m%d",&mytm);
//  strcpy(date,"20080423");
  strcat(fname,date);
  strcat(fname,".txt");
  printf("%s\n",fname);
  fp = fopen("gnuplotoptions.txt","w+");
  if(fp!=NULL)
  {
    fprintf(fp,"set autoscale\n");
    fprintf(fp,"unset log\n");
    fprintf(fp,"unset label\n");
    fprintf(fp,"set xtic auto\n");
    fprintf(fp,"set ytic auto\n");
    fprintf(fp,"set grid\n");
    fprintf(fp,"set xlabel \"Tiempo\"\n");
    fprintf(fp,"set ylabel \"|Vprom| (Volts)\"\n");
    fprintf(fp,"set title \"Santa Filomena %s\"\n", date);
    fprintf(fp,"set terminal png size 6000,600\n");
    //fprintf(fp,"set terminal png small color\n");
    fprintf(fp,"set output\n");
    fprintf(fp,"plot \"%s\" using 4 with lines lw 2 title \"Voltaje de Tierra a Riel\"\n",fname);
    fclose(fp);
  }
  char command[500];
  char target[20];
  strcpy(target,date);
  strcat(target,".png");
//  sprintf(command,"gnuplot -persist gnuplotoptions.txt",target);
  snprintf(command,sizeof(command),"gnuplot gnuplotoptions.txt > %s",target);
  printf("%s\n",command);
  system(command);
  return 0;
}
