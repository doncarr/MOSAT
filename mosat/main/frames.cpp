#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<ctype.h>

#include "rtcommon.h"
#include "arg.h"

void header(const char Y[10], const char M[10], const char D[10],const char  P[100],const char st[50],const char st2[10],const char color[20],const char img[20],const char dir[20],const char pdc[20][10],int t)
{
 struct tm mytm;
 time_t mytime;
 FILE *fd;
 char Path[200];
 localtime_r(&mytime,&mytm);
 int weekday = mytm.tm_wday;
 int month_day = mytm.tm_mday;
 int month = mytm.tm_mon;
 int flag=0;
 FILE *hd;
 const char *festivo_fname = "/var/www/html/react/log/festivo.txt";
 hd=fopen(festivo_fname,"r");
 if(hd == NULL)
 {
   printf("Cannot read the file festivo.txt");
   perror(festivo_fname);
   exit(1);
 }
 else
 {
  int cont=0,cont2=0,flag=0;
  char month_fest[20][10],day_fest[30][30],c;
  while(!feof(hd))
  {
   fread(&c,sizeof(c),1,hd);
   if(!isspace(c))
   {
    if(!flag)
     month_fest[cont][cont2]=c;
    else
     day_fest[cont][cont2]=c;
    cont2++;
   }
   else
   {
    if(!flag)
    {
     flag=1;
     month_fest[cont][cont2]='\x0';
    }
    else
    {
     flag=0;
     day_fest[cont][cont2]='\x0';
     cont++;
    }
    cont2=0;
   }
  }
  fclose(hd);
  for(cont2=0;cont2<cont;cont2++)
  {
   if(month==atoi(month_fest[cont2]))
    if(month_day==atoi(day_fest[cont2]))
     flag=1;
  }
 }
 strcpy(Path,P);
 strcat(Path,dir);
 strcat(Path,"/");
 strcat(Path,dir);
 strcat(Path,"_header.html");
 fd=fopen(Path,"w+");
 if (fd == NULL)
 {
   perror(Path);
   exit(1);
 }
 fprintf(fd,"<HTML>\n");
 fprintf(fd," <HEAD>\n");
 fprintf(fd,"  <META HTTP-EQUIV=\"refresh\" CONTENT=\"30\">\n");
 fprintf(fd," </HEAD>\n");
 fprintf(fd," <BODY>\n");
 fprintf(fd," <center>\n");
 if(flag)
 {
  fprintf(fd,"<B>Tipo D&iacute;a: Festivo</B>\n");
 }
 else
 {
  switch(weekday)
  {
   case 0:
    fprintf(fd,"<B>Tipo D&iacute;a: Domingo</B>\n");
   break;
   case 6:
    fprintf(fd,"<B>Tipo D&iacute;a: S&aacute;bado</B>\n");
   break;
   default:
    fprintf(fd,"<B>Tipo D&iacute;a: Laboral</B>\n");
   break;
  }
 }
 fprintf(fd,"  <BR>");
  fprintf(fd,"  Porcentaje de Cumplimineto: %s %% \n",pdc[t]); 
 fprintf(fd,"  <TABLE style=\"text-align: left; width: 306px;\" border=\"2\" cellpadding=\"1\" cellspacing=\"5\">\n");
 fprintf(fd,"   <TR>\n");
 fprintf(fd,"    <TD colspan=\"2\" style=\"text-align: center; vertical-align: center;\">");
 fprintf(fd," <IMG alt=\"%s\" src=\"../%s.jpg\" style=\"width:63px; height:63px;\"><BR>\n",img,img);
 fprintf(fd,"    </TD>\n");
 fprintf(fd,"    <TD colspan=\"5\"style=\"background-color: rgb(%s); text-align: center; vertical-align: center\"><big><big>%s",color,st);
 if(t)
 fprintf(fd," %s",st2);
 fprintf(fd,"<BR>(%s-%s-%s)</big></big><BR>\n",Y,M,D);
 fprintf(fd,"    </TD>\n");
 fprintf(fd,"   </TR>\n");
 fprintf(fd,"   <TR>\n");
 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\">No.<BR>Sa-<BR>lida<br>\n",color);
 fprintf(fd,"    </TH>\n");
 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\">No.<BR>de<BR>Tren<BR>\n",color);
 fprintf(fd,"    </TH>\n");
 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\">Hora<BR>Progra.\n",color);
 fprintf(fd,"    </TH>\n");
 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\"><BIG> Hora <BR> Real </BIG>\n",color);
 fprintf(fd,"    </TH>\n");
 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\">Dife-<BR>rencia<BR>vs<BR>meta<BR>\n",color);
 fprintf(fd,"    </TH>\n");
 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\">Dif.<BR>vs<BR>Sal. Ant.<BR>\n",color);
 fprintf(fd,"    </TH>\n");

 fprintf(fd,"    <TH style=\"background-color: rgb(%s);vertical-align: center; text-align: center;\">Tipo de<BR>Interrup-<BR>ci&oacute;n\n",color);
 fprintf(fd,"    </TH>\n");
 fprintf(fd,"   </TR>\n");
 fprintf(fd,"  </TABLE>\n");
 fprintf(fd," </center>");
 fprintf(fd," </BODY>\n");
 fprintf(fd,"</HTML>");
 fclose(fd);
}
void get_pdc(char pdc[20],const char dir[20])
{
 time_t tt=time(NULL);
 struct tm stm;
 localtime_r(&tt,&stm);
 char path[160]="/var/www/html/react/log/",day[20],c[4];
 int cont=0;
 strftime(day,sizeof(day),"%d",&stm);
 strcat(path,dir);
 strcat(path,"pdc.txt");
 FILE *pc;
 pc=fopen(path,"r");
 if (pc == NULL)
 {
   perror(path);
   exit(1);
 }
 if(pc!=NULL)
 {
  while(!feof(pc))
  {
   fread(c,1,1,pc);
   if(!isspace(c[0]))
   {
    pdc[cont]=c[0];
    cont++;
   }
   else
   {
    pdc[cont]='\x0';
    cont=0;
    c[0]='\x0';
    if(!strcmp(pdc,day))
    {
     while(!isspace(c[0]))
     {
      fread(c,1,1,pc);
      pdc[cont]=c[0];
      cont++;
     }
     pdc[cont]='\x0';
     break;
    }
   }
  }
  fclose(pc);
 }
}

void frame_generator(int refresh,const char L[3])
{
 FILE *fr;
 char Day[200],Day2[200],D[20],M[20],Y[20],pdc[20][10];
 time_t tim;
 struct tm tim1;
 tim = time(NULL);
 localtime_r(&tim,&tim1);
 strftime(Day,sizeof(Day),"%Y%m%d",&tim1);
 strftime(D,sizeof(D),"%d",&tim1);     D[2]='\x0';
 strftime(M,sizeof(M),"%m",&tim1);     M[2]='\x0';
 strftime(Y,sizeof(Y),"%Y",&tim1);     Y[4]='\x0';

 get_pdc(pdc[0],"tetlan");
 get_pdc(pdc[1],"periferico_sur");
 get_pdc(pdc[2],"juarez2");
 
 //Added one line here to fix percent completion problem
 get_pdc(pdc[3],"periferico_norte");

 strncpy(Day2,"/var/www/html/react/log/",24);
 Day2[24]='\x0';
 header(Y,M,D,Day2,"Perif&eacute;rico","Sur","49,112,207","per_sur","periferico_sur",pdc,1);

 strncpy(Day2,"/var/www/html/react/log/",24);
 Day2[24]='\x0';
 header(Y,M,D,Day2,"Tetl&aacute;n","","74,200,48","tetlan","tetlan",pdc,0);

 strncpy(Day2,"/var/www/html/react/log/",24);
 Day2[24]='\x0';
 header(Y,M,D,Day2,"Ju&aacute;rez","2","74,200,48","juarez2","juarez2",pdc,2);

 //Added 3 lines here to fix percent completion problem
 strncpy(Day2,"/var/www/html/react/log/",24);
 Day2[24]='\x0';
 header(Y,M,D,Day2,"Perif&eacute;rico","Norte","49,112,207","per_norte","periferico_norte",pdc,3);

 strncat(Day2,"Reporte_Hoy",35);
 Day2[35]='\x0';
 strncat(Day2,"_",36);
 Day2[36]='\x0';
 strncat(Day2,L,38);
 Day2[38]='\x0';
 strncat(Day2,".html",43);
 Day2[43]='\x0';
 fr = fopen(Day2,"w+");
 if (fr == NULL)
 {
   perror(Day2);
   exit(1);
 }
 fprintf(fr,"<HTML>\n");
 fprintf(fr," <TITLE>\n\t\tSalida de Trenes\n </TITLE>\n");
 fprintf(fr," <meta http-equiv=\"refresh\" content=\"%i\">\n",refresh);
 fprintf(fr," <meta http-equiv=\"pragma\" content=\"no-cache\">\n");
 fprintf(fr,"  <FRAMESET rows = \"35%%,65%%\" cols = \"50%%,50%%\">\n");
 if(strcmp(L,"L2")==0)
 {
  fprintf(fr,"   <FRAME src=\"juarez2/juarez2_header.html\">\n");
  fprintf(fr,"   <FRAME src=\"tetlan/tetlan_header.html\">\n");
  fprintf(fr,"   <FRAME src=\"juarez2/%s_report_juarez2.html\">\n",Day);
  fprintf(fr,"   <FRAME src=\"tetlan/%s_report_tetlan.html\">\n",Day);
 }
 else
 {
  fprintf(fr,"   <FRAME src=\"periferico_sur/periferico_sur_header.html\">\n");
  fprintf(fr,"   <FRAME src=\"periferico_norte/periferico_norte_header.html\">\n");
  fprintf(fr,"   <FRAME src=\"periferico_sur/%s_report_p_sur.html\">\n",Day);
  fprintf(fr,"   <FRAME src=\"periferico_norte/%s_report_p_norte.html\">\n",Day);
 }
 fprintf(fr,"  </FRAMESET>\n");
 fprintf(fr,"</HTML>");
 fclose(fr);
}


int main (int argc, char **argv)
{
  if (argc < 2)
  {
    printf("You must specify the time between refreshes on the command line\n");
    exit(1);
  }

  const char *fname = "./record_config.txt";
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    exit(0);
  }

  int n_stations=0;
  const char *station_dirs[20];

  char line[300];
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argcs;
    char *argvs[25];

    printf("Line %d: %s\n", i, line);
    if (line[0] == '#') continue;
    safe_strcpy(tmp, line, sizeof(tmp));
    argcs = get_delim_args(tmp, argvs, '|', 2);
    if (argcs == 0)
    {
      continue;
    }
    if (argcs == 2)
    {
      station_dirs[n_stations]=strdup(argvs[1]);
      printf("Station dir = %s\n",station_dirs[n_stations]);
      n_stations++;
      if (n_stations>=20)
      {
        printf("Max stations exceeded\n");
        break;
      }
    }
  }


  int ref = atoi(argv[1]);
  printf("Generando reporte L2... ");
  frame_generator(ref,"L2");
  printf("Hecho!\n");
  printf("Generando reporte L1... ");
  frame_generator(ref,"L1");
  printf("Hecho!\n");
  return 0;
}

/*
int main (int argc, char **argv)
{
 if (argc != 2)
 {
   printf("You must specify the time between refreshes on the command line\n");
   exit(1);
 }
 int n_stations;
 const char *station_dirs[20];

 int ref = atoi(argv[1]);
 printf("Generando reporte L2... ");
 frame_generator(ref,"L2");
 printf("Hecho!\n");
 printf("Generando reporte L1... ");
 frame_generator(ref,"L1");
 printf("Hecho!\n");
 return 0;
}
 */
