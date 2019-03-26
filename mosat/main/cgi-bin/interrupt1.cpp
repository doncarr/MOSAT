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


int main(int argc, char *argv[])
{

  printf("Content-type: text/html\n\n");

  printf("<html>");
  printf("<head>");
  printf("<meta http-equiv=\"content-type\"");
  printf("content=\"text/html; charset=ISO-8859-1\">");
  printf("<title>Agregar Interrupci&oacute;n para %s</title>\n",argv[3]);
/***************************************************************************************/
  printf("<script language=\"javascript\">\n");
  printf("function openWin(url)\n");
  printf("{\n");
  printf("newWin = window.open(url,'newWin','height=200,width=400,resizable=yes,scrollbars=yes,status=no,dependent=yes');\n");
  printf("newWin.focus();\n");
  printf( "}\n");
  printf( "</script>\n");
/***************************************************************************************/

  printf("</head>");

  printf("%s\n", "<body style=\"color: rgb(0, 0, 0); background-color: rgb(204, 204, 204);\"");
  printf("%s\n", "link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">");

  if (argc<4)
  {
    //perror("reactqid.txt");
    printf("<h2>Agregar Interrupci&oacute;n</h2>");
    printf("Argumentos insuficientes<br>\n");
    fflush(stdout);
    printf("%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); return false\">Cerrar ventana</a>] </center>");
    exit(0);
  }
  printf("<h2>Agregar Interrupci&oacute;n a la salida %s</h2>",argv[1]);
  if (false)
  {
    //perror("reactqid.txt");
    printf("On no, what happened!<br>\n");
    fflush(stdout);
    printf("%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); return false\">Cerrar ventana</a>] </center>");
    exit(0);
  }

/*  for (int i=1; i < argc; i++)
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
  char *interr[7]={"1) Operaci&oacute;n","2) Perturbaci&oacute;n","3) Aver&iacute;a en M/R","4) Aver&iacute;a en I/F","5) Causas ajenas","6) Otros","7) Salir"};
  for(int x=0;x<7;x++)
          printf("<br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); openWin('/cgi-bin/interrupt2?%d+%s+%s+%s+%s'); return false\">%s</a>]</center>\n",x+1,argv[1],interr[x],argv[2],argv[3],interr[x]);

/*
"<a href=\"#\" onclick=\"openWin('/cgi-bin/interrupt1?%d');return false;\" class=\"menu8\">%d</a>"
*/
}


