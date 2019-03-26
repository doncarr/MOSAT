

#include <stdio.h>

int main(int argc, char *argv[])
{
  FILE *fp1= fopen("temp_hum_sim.txt", "r");
  FILE *fp2 = fopen("out_temp_hum_sim.txt", "w");

  char line[300];
  int count = 0;
  int cnt1, cnt2;
  float tmp1, tmp2, hum1, hum2, tdif, hdif;

  fgets(line, sizeof(line), fp1);
  sscanf(line, "%d\t%f\t%f", &cnt1, &tmp1, &hum1);

  for (int i=0; NULL != fgets(line, sizeof(line), fp1); i++)
  {
    sscanf(line, "%d\t%f\t%f", &cnt2, &tmp2, &hum2);
    tdif = tmp2 - tmp1; 
    hdif = hum2 - hum1; 
    printf("%d  %0.1f  %0.1f  %0.1f  %0.1f  %0.1f  %0.1f\n", 
              count, tmp1, tmp2, hum1, hum2, tdif, hdif);
    for(int j=0; j < 4; j++)
    {
      float pct = float(j) * 0.25; 
      float tmp = tmp1 + (pct * tdif);
      float hum = hum1 + (pct * hdif);
      fprintf(fp2, "%d\t%0.1f\t%0.1f\n", count, tmp, hum);
      printf("------- %d\t%0.1f\t%0.1f\n", count, tmp, hum);
      count++;
    }
    cnt1 = cnt2; tmp1 = tmp2; hum1 = hum2;
  }

}
