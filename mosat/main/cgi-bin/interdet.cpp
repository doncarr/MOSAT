/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr 

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*************************************************************************/



#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/msg.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


/*************************************************************************/


void strip_slash(char *s)
{
  int l = strlen(s);
  for (int i=0; i < l; i++) 
  {
    if (s[i] == '\\')
    {
      for (int j=i; j < l; j++)
      {
        s[j] = s[j+1];
      }
    }
  }
}


/**************************************************************************/
int get_int (char argv[],char n_exit[50][200],char int_type[50][200])
{
 time_t mtmt;
 struct tm mtm;
 FILE *file;
 char path[100]="/var/www/html/react/log/",buf[50][200],date[20];
 int w,x=0,y,z,l=strlen(path);
 strncat(path,argv,l+strlen(argv));
 strcat(path,"/");
 mtmt = time(NULL);
 localtime_r(&mtmt,&mtm);
 strftime(date,sizeof(date),"%Y%m%d",&mtm);
 strcat(path,date);
 l=strlen(path);
 strncat(path,"_int.txt",l+8);
 file = fopen(path,"r");
 if(file != NULL)
 {
  while(!feof(file))
  {
   fgets(buf[x],sizeof(buf[x]),file);
   x++;
  }
  fclose(file);
  y=0;
  x--;
  while(y<x)
  {
   z=0;
   w=0;
   while(buf[y][z]!='\t')
   {
    if(isdigit(buf[y][z]))
    {
     n_exit[y][w]=buf[y][z];
     w++;
     z++;
    }
    else
     z++;
   }
   n_exit[y][w]='\x0';
   w=0;
   z++;
   while(buf[y][z]!='\n')
   {
    if(isdigit(buf[y][z]))
    {
     int_type[y][w]=buf[y][z];
     w++;
     z++;
    }
    else
     z++;
   }
   int_type[y][w]='\x0';
   y++;
  }
  for(y=0;y<x;y++)
   for(z=y+1;z<x;z++)
    if(!strcmp(n_exit[y],n_exit[z]))
    {
     for(w=y;w<x-1;w++)
     {
      strcpy(n_exit[w],n_exit[w+1]);
      strcpy(int_type[w],int_type[w+1]);
     }
     x--;
    }
 }
 return x;
}
/*************************************************************************/


int main(int argc, char *argv[])
{

  printf("Content-type: text/html\n\n");

  printf("<html>\n");
  printf("<head>\n");
  printf("<meta http-equiv=\"content-type\"\n");
  printf("content=\"text/html; charset=ISO-8859-1\">\n");
  printf("<title>Detalles de Interrupci&oacute;n</title>\n",argv[5]);

  printf("</head>\n");

  printf("%s\n", "<body style=\"color: rgb(0, 0, 0); background-color: rgb(204, 204, 204);\"\n");
  printf("%s\n", "link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">\n");
  printf("<h2>%s</h2>\n",argv[3]);
  printf("<br><br>N&uacute;mero de Salida %s<br>\n",argv[1]);
  if (argc<3)
  {
    //perror("reactqid.txt");
    printf("On no, the arguments are wrong!<br>\n");
    fflush(stdout);
    printf("%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); return false\">Cerrar ventana</a>] </center>");
    exit(0);
  }
/* for (int i=1; i < argc; i++)
  {
    strip_slash(argv[i]);
    printf("%s<br>\n", argv[i]);
    char buf[50];
    //printf("Enter a command ('q' to quit): ");
    //fgets(buf, sizeof(buf), stdin);
    //if (buf[0] == 'q')
    //{
     // break;
    //}
  }*/
/*****************************************************************************/
  char n_exit[50][200],int_type[50][200];
  char *inter[6]={"Operaci&oacute;n","Perturbaci&oacute;n","Aver&iacute;a en M/R","Aver&iacute;a en I/F","Causas Ajenas","Otros"};
  int x;
  x = get_int(argv[2],n_exit,int_type);
  for(int a = 0;a<x;a++)
  {
   if(strcmp(n_exit[a],argv[1])==0)
   {
    printf("<br>Tipo de Interrupci&oacute;n: %s<br>\n",inter[atoi(int_type[a])-1]);
   }
  }
  int argv1 = atoi(argv[1]);
//  printf("<br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); openWin('/cgi-bin/interrupt2?%d+%s+%s+%s+%s'); return false\">%s</a>]</center>\n",x+1,argv[1],interr[x],argv[2],argv[3],interr[x]);
  printf("<br>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); openWin('/cgi-bin/interrupt2?%d+%s+%s'); return false\" class=\"menu8\">Borrar Interrupci&oacute;n</a>]\n",argv1,argv[2],argv[3]);
/*****************************************************************************/
  printf("%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\" window.close(); return false\">Cerrar ventana</a>] </center>");
/*
"<a href=\"#\" onclick=\"openWin('/cgi-bin/interrupt1?%d');return false;\" class=\"menu8\">%d</a>"
*/
}
