

#include <stdio.h>

struct fan_t 
{
  int modbus_id;
  int on_ch;
  int off_ch;
  int is_on_ch;
  int cancel_ch;
};

struct silo_t
{
  const char *id;
  bool is_bodega;
  int n_fans;
  fan_t *fans;
};

struct site_t
{
  const char *name;
  const char *abr;
  int n_silos;
  int modbus_id;
  int temp_amb_ch;
  int hum_rel_ch;
  silo_t *silos;
};

fan_t fans1[] = {{1, 0, 1, 0, 16}, {1, 2, 0, 1, 17}};
fan_t fans2[] = {{1, 3, 4, 2, 18}, {1, 5, 4, 3, 19}};
fan_t fans3[] = {{1, 6, 7, 4, 20}, {1, 8, 6, 5, 21}};
fan_t fans4[] = {{1, 9, 10, 6, 22}, {1, 11, 9, 7, 23}};
fan_t fans5[] = {{1, 12, 13, 8, 24}, {1, 14, 12, 9, 25}};

silo_t silos[] = 
{
  {"12", false, 2, fans1}, 
  {"16", false, 2, fans2},
  {"18", false, 2, fans3}
}; 

site_t sites[] = 
{{"Correrepe", "COR", 3, 1, 0, 1, silos}, {"Cooperativo", "COR", 3, 1, 0, 1, silos} };

int main(int argc, char *argv[])
{

//#    Tag (string max 25)
//#    Description (string max 50)
//#    Driver Number (char max 25)
//#    Card (char max 25)
//#    Channel (char max 25)
//#    Low Description (string max 12)
//#    High Description (string max 12)
//#    Alarm State (drop down {high, low, none})
//#    Shutdown State  (drop down {high, low, none})

    
  printf("%s:%d\n", __FILE__, __LINE__);    
    
  for (int i=0; i < int(sizeof(sites)/sizeof(sites[0])); i++)
  {
    printf("  site[%d]: name: %s, abr: %s, n_silos: %d\n", 
                   i, sites[i].name, sites[i].abr, sites[i]. n_silos); 
    printf("  %s:%d, i = %d\n", __FILE__, __LINE__, i);    
    for (int j=0; j < sites[i].n_silos; j++)
    {
      printf("    %s:%d, i = %d, j = %d\n", __FILE__, __LINE__, i, j);    
      for (int k=0; k < sites[i].silos[j].n_fans; k++)
      {
        printf("      %s:%d, i = %d, j = %d, k = %d\n", __FILE__, __LINE__, i, j, k);    
        printf("        %s_%s_fan_%d_on_do\n",
          sites[i].abr, 
          sites[i].silos[j].id, 
          k);
        printf("        %s_%s_fan_%d_off_do\n",
          sites[i].abr, 
          sites[i].silos[j].id, 
          k);
        printf("        %s_%s_fan_%d_is_on_di\n",
          sites[i].abr, 
          sites[i].silos[j].id, 
          k);
          //sites[i].silos[j].fans[k].
          //sites[i].silos[j].fans[k].
          //sites[i].silos[j].fans[k].
        printf("        %s_%s_fan_%d_cancel_di\n",
          sites[i].abr, 
          sites[i].silos[j].id, 
          k);
      }
    }
  }
}

