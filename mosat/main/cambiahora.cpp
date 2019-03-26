
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "rtcommon.h"

/*********************************************************************/

int main(int argc, char *argv[])
{

  char cmd[800];

  char src1[300];
  char dest1[300];
  char src2[300];
  char dest2[300];

  const char *norte = "/home/react/public_html/log/periferico_norte";
  const char *sur = "/home/react/public_html/log/periferico_sur";
  const char *juarez2 = "/home/react/public_html/log/juarez2";
  const char *tetlan = "/home/react/public_html/log/tetlan";

  const char *es1;
  const char *es2;
  const char *dest_name;

  if (argc < 2)
  {
    printf("Deberia poner la linea y despues el archivo para usar\n");
    printf("ejemplo: cambiahora 1 laboral12.tx\n");
    exit(1);
  }


  if (0 == strcmp("1", argv[1]))
  {
    es1 = norte;
    es2 = sur;
  }
  else if (0 == strcmp("2", argv[1]))
  {
    es1 = juarez2;
    es2 = tetlan;
  }
  else
  {
    printf("La linea debe de ser 1 o 2\n");
    printf("ejemplo: cambiahora 1 laboral12.tx\n");
    exit(1);
  }

  if (0 == strncmp("laboral", argv[2], 7))
  {
    printf("OK, cambiando laboral para linea %s. . . \n", argv[1]);
    dest_name = "laboral.txt";
  }
  else if (0 == strncmp("sabado", argv[2], 6))
  {
    printf("OK, cambiando sabado para linea %s. . . \n", argv[1]);
    dest_name = "sabado.txt";
  }
  else if (0 == strncmp("domingo", argv[2], 7))
  {
    printf("OK, cambiando domingo para linea %s. . . \n", argv[1]);
    dest_name = "domingo.txt";
  }
  else
  {
    printf("El archio deberia empezar con 'laboral', 'sabado', o 'domingo'\n");
    printf("ejemplo: cambiahora 1 laboral12.tx\n");
    exit(1);
  }


  snprintf(src1, sizeof(src1), "%s/%s", es1, argv[2]);
  snprintf(dest1, sizeof(dest1), "%s/%s", es1, dest_name);
  if (!file_exists(src1))
  {
    printf("El archio no existe: %s\n", src1);
    exit(1);
  } 

  snprintf(src2, sizeof(src2), "%s/%s", es2, argv[2]);
  snprintf(dest2, sizeof(dest2), "%s/%s", es2, dest_name);
  if (!file_exists(src2))
  {
    printf("El archio no existe: %s\n", src2);
    exit(1);
  } 

  snprintf(cmd, sizeof(cmd), "cp -v %s %s", src1, dest1);
  system(cmd);
  //printf("%s\n", cmd);

  snprintf(cmd, sizeof(cmd), "cp -v %s %s", src2, dest2);
  system(cmd);
  //printf("%s\n", cmd);

  printf("Terminado, todo bien\n");

}

/*********************************************************************/

