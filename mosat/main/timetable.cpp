
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "tt.h"

struct tdinfo_t
{
  int above;
  int train_number;
  int square;
  time_t depart_time;
};

struct section_info_t
{
  const char *name;
  int n;
  double wait_time;
  double times[20];
};

section_info_t section_info =
{
  "Linea 1 - Tramo Sur",
  10,
  8.0,
  {125, 102, 77, 90, 74, 87, 68, 91, 102, 71}
};

int ntd;
tdinfo_t td[100];


int square_number(double difference, section_info_t *sinfo)
{
  double total_time = 0.0;
  int ns = 0;
  for (int i=0; i < sinfo->n; i++)
  {
    if (difference < (total_time + sinfo->times[i]))
    {
      double fraction = difference - total_time;
      double f25 = sinfo->times[i] / 4.0;
      return ns + 1 + int(fraction / f25);
    }
    if (difference < (total_time + sinfo->times[i]) + 8.0)
    {
      return ns + 5;
    }
    ns += 5;
    total_time += sinfo->times[i] + 8.0;
  }

   return 500;
}


/***************************************************************************/

void gen_html(FILE *fp, const char *background, int top, int left, 
             const char *iname, int n_image, int itop[], int ileft[], 
             int textleft[], int texttop[], const char thetext[][25])
{
  fprintf(fp, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
  fprintf(fp, "<html>\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "<META HTTP-EQUIV=\"refresh\" CONTENT=\"15\">\n");
  fprintf(fp, "<meta http-equiv=\"pragma\" content=\"no-cache\">\n");
  fprintf(fp, "  <meta http-equiv=\"content-type\"\n");
  fprintf(fp, " content=\"text/html; charset=ISO-8859-1\">\n");
  fprintf(fp, "  <title>Linea 1 - Tramo SUR</title>\n");
  fprintf(fp, "</head>\n");
  fprintf(fp, "<body style=\"color: rgb(0, 0, 0); background-color: rgb(204, 204, 204);\"\n");
  fprintf(fp, " link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">\n");
  time_t now;
  now = time(NULL);
  char buf1[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf1, sizeof(buf1), "%Y-%m-%d %T", &mytm);
  fprintf(fp, "<h1>Linea 1 - Tramo SUR (%s)</h1>\n", buf1);
  fprintf(fp, "\n");
  fprintf(fp, "\n");
  fprintf(fp, "<div style=\"position:absolute; top:%dpx; left:%dpx;\">\n", top, left);
  fprintf(fp, "        <img SRC=%s>\n", background);
  fprintf(fp, "</div>\n");

  for (int i=0; i < n_image; i++)
  {
    fprintf(fp, "\n");
    fprintf(fp, "<div style=\"position:absolute; top:%dpx; left:%dpx; z-index:2\">\n", top + itop[i], left + ileft[i]);
    fprintf(fp, "        <img SRC=%s>\n", iname);
    fprintf(fp, "</div>\n");
    fprintf(fp, "<div style=\"position:absolute; top:%dpx; left:%dpx; z-index:2\">\n", top + texttop[i], left + textleft[i]);
    fprintf(fp, "        %s\n", thetext[i]);
    fprintf(fp, "</div>\n");
  }

  fprintf(fp, "\n");
  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");
}

/**********************************************************************/
void gen_display(FILE *fp, const char *background, int top, int left, const char *iname, 
                tdinfo_t td[], int n)
{
  int itop[50];
  int ileft[50];
  int texttop[50];
  int textleft[50];
  char thetext[50][25];
  for (int k=0; k < n; k++)
  {
    if (td[k].above == 0)
    {
      itop[k] = 180;
      texttop[k] = 180 - 36;
    }
    else
    {
      itop[k] = 222;
      texttop[k] = 222 + 30;
    }
    ileft[k] = (int)((((double) td[k].square / 45.0) * (969.0 - 22.0)) + 22.0);
    textleft[k] = ileft[k] - 10;
    char buf[30];
    struct tm mytm;
    localtime_r(&td[k].depart_time, &mytm);
    strftime(buf, sizeof(buf), "%T", &mytm);
    snprintf(thetext[k], sizeof(thetext[k]), "#%d %s", td[k].train_number, buf);
  }
  gen_html(fp, background, top, left, iname, n, 
       itop, ileft, textleft, texttop, thetext);
}

/**********************************************************************/

#define NORMAL_DEPARTURE (0)
#define MISSING_DEPARTURE (1)
#define EXTRA_DEPARTURE (2)


int decide_status(time_t programmed[], int prog_idx, 
                    time_t actual[], int actual_idx, int max)
{
  if ((actual[actual_idx] - programmed[actual_idx]) > 300) 
  {
    return MISSING_DEPARTURE;
  }
  else
  {
    return NORMAL_DEPARTURE;
  }
  int total1 = 0;
  int total2 = 0;
  int total3 = 0;
  int n = 0;
  for (int i=0; i < 1; i++)
  {
    if ((i + prog_idx + 1) >= max)
    {
      break;
    }
    if ((i + actual_idx + 1) >= max)
    {
      break;
    }
    time_t pro = programmed[i + prog_idx];
    time_t pro1 = programmed[i + prog_idx + 1];
    time_t act = actual[i + actual_idx]; 
    time_t act1 = actual[i + actual_idx + 1]; 
    if ((pro == 0) || (pro1 == 0) || (act == 0) || (act1 == 0))
    {
      break;
    }
    total1 += abs(pro - act);
    total2 += abs(pro - act1);
    total3 += abs(pro1 - act);
    n++;
  }
  if (n < 1)
  {
    return NORMAL_DEPARTURE;
  }
  total1 = abs(total1);
  total2 = abs(total2);
  total3 = abs(total3);
  if (total1 <= total2)
  {
    if (total1 <= total3)
    {
      return NORMAL_DEPARTURE;
    }
    else
    {
      printf("Missing Departure at %d %d\n", prog_idx, actual_idx);
      return MISSING_DEPARTURE;
    }
  }
  else
  {
    if (total2 <= total3)
    {
      printf("Extra Departure at %d %d\n", prog_idx, actual_idx);
      return EXTRA_DEPARTURE;
    }
    else
    {
      printf("Missing Departure at %d %d\n", prog_idx, actual_idx);
      return MISSING_DEPARTURE;
    }
  }
  return NORMAL_DEPARTURE;

}
/**************************************************************************/
int get_int (char argv[],char n_exit[][200],char int_type[][200],int size)
{
 time_t mtmt;
 struct tm mtm;
 FILE *file;
 char path[200],buf[50][200],date[20];
 int w,x=0,y,z;
/* strncat(path,argv,l+strlen(argv));
 strcat(path,"/");*/
 mtmt = time(NULL);
 localtime_r(&mtmt,&mtm);
 strftime(date,sizeof(date),"%Y%m%d",&mtm);
/* strcat(path,date);
 l=strlen(path);
 strncat(path,"_int.txt",l+8);
 puts(path);*/
 snprintf(path,sizeof(path),"/var/www/html/react/log/%s/%s_int.txt",argv,date);
 printf("%s\n",path);
 file = fopen(path,"r");
 if (file == NULL)
 {
  printf("%s:%d: ",__FILE__,__LINE__);
  printf("No interruption file %s yet \n",path);
  //exit(1);
 }
 else
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
    }
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
    }
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
      snprintf(n_exit[w], size, "%s", n_exit[w+1]);
      /*strcpy(n_exit[w],n_exit[w+1]);
      strcpy(int_type[w],int_type[w+1]);*/
      snprintf(int_type[w], size, "%s", int_type[w+1]);
     }
     x--;
    }
  char aux[200];
  for(y=0;y<x;y++)
  {
   for(z=y+1;z<x;z++)
   {
    if(atoi(n_exit[y])>atoi(n_exit[z]))
    {
     /*strcpy(aux,n_exit[y]);
     strcpy(n_exit[y],n_exit[z]);
     strcpy(n_exit[z],aux);
     strcpy(aux,int_type[y]);
     strcpy(int_type[y],int_type[z]);
     strcpy(int_type[z],aux);*/
     snprintf(aux,sizeof(aux),"%s",n_exit[y]);
     snprintf(n_exit[y],200,"%s",n_exit[z]);
     snprintf(n_exit[z],size,"%s",aux);
     snprintf(aux,sizeof(aux),"%s",int_type[y]);
     snprintf(int_type[y],size,"%s",int_type[z]);
     snprintf(int_type[z],size,"%s",aux);
    }
   }
  }
  remove(path);
  file = fopen(path,"w+");
  if (file == NULL)
  {
    printf("%s:%d: ",__FILE__,__LINE__);
    perror(path);
    exit(1);
  }
  for(y=0;y<x;y++)
  {
   fprintf(file,"%s\t%s\n",n_exit[y],int_type[y]);
  }
  fclose(file);
 }
 return x;
}
/*************************************************************************/

void print_it(FILE *fp, int train[], time_t programmed[], 
              time_t actual[], int interuption[], int n_times,
               char *station,int week_day,char *dir)
{
  float pdc=0,total_departures=0,des,sum=0,tms[300],pos,neg,r_prom;
  char buf1[30];
  char buf2[30];
  char buf3[30];
  char buff[30];
  struct tm mytm;
  int hr_dif,min_dif,sec_dif,interflag;
  char D[5],M[30];
  int interval[10]={0,0,0,0,0,0,0,0,0,0};
  int intervalcont[2]={0,0},ln=0;
  if(!strcmp(dir,"periferico_sur"))
  {
   if(week_day!=0&&week_day!=6)
    ln=1;
  }
  if(!strcmp(dir,"tetlan"))
  {
   if(week_day!=0&&week_day!=6)
    ln=2;
  }
  fprintf(fp, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
  fprintf(fp, "<html>\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "  <meta content=\"text/html; charset=ISO-8859-1\"\n");
  fprintf(fp, " http-equiv=\"content-type\">\n");
  fprintf(fp, "  <title>Salide de Trenes</title>\n");
  fprintf(fp, "  <meta http-equiv=\"refresh\" content=\"10\">\n");
  fprintf(fp, "  <meta http-equiv=\"pragma\" content=\"no-cache\">\n");
/***************************************************************************************/
  fprintf(fp,"<script language=\"javascript\">\n");
  fprintf(fp,"function openWin(url)\n");
  fprintf(fp,"{\n");
  fprintf(fp,"newWin = window.open(url,'newWin','height=400,width=450,resizable=yes,scrollbars=yes,status=no,dependent=yes');\n");
  fprintf(fp,"newWin.focus();\n");
fprintf(fp, "}\n");
  fprintf(fp, "</script>\n");
/***************************************************************************************/


  fprintf(fp, "</head>\n");
  fprintf(fp, "<body>\n");
  fprintf(fp, "<center>\n");
  fprintf(fp, "<table style=\"text-align: left; width: 300px; height: 222px;\" border=\"1\"\n");
  fprintf(fp, " cellpadding=\"0\" cellspacing=\"1\">\n");


  fprintf(fp, "<tr>\n");
  fprintf(fp, "<td colspan=\"2\" style=\"text-align: center; vertical-align: center;\"><img\n");
  fprintf(fp, "alt=\"slogo\" src=\"../slogo.jpg\" style=\"width: 75px; height: 75px;\"><br>\n");
  fprintf(fp, "</td>\n");
  fprintf(fp, "<td colspan=\"5\" style=\"text-align: center; vertical-align: center;\"><big><big><big>%s<br>\n", station);

  localtime_r(&programmed[0], &mytm);
  strftime(buf1, sizeof(buf1), "%F", &mytm);
  fprintf(fp, "(%s)", buf1);
  fprintf(fp, "</big></big></big><br>\n");
  fprintf(fp, "</td>\n");
  fprintf(fp, "</tr>\n");

  fprintf(fp, "    <tr>\n");
  fprintf(fp,"       <th style=\"vertical-align: center; text-align: center;\">No. <br> Sa- <br> lida<br>\n");
  fprintf(fp,"       </th>");
  fprintf(fp, "      <th style=\"vertical-align: center; text-align: center;\">No. <br> de <br>\n");
  fprintf(fp, "Tren<br>\n");
  fprintf(fp, "      </th>\n");
  fprintf(fp, "      <th style=\"vertical-align: center; text-align: center;\">Hora <br>\n");
  fprintf(fp, "Prog.<br>\n");
  fprintf(fp, "      </th>\n");
  fprintf(fp, "      <th style=\"vertical-align: center; text-align: center;\">Hora <br>\n");
  fprintf(fp, "Real<br>\n");
  fprintf(fp, "      </th>\n");
  fprintf(fp, "      <th style=\"vertical-align: center; text-align: center;\">Dife-<br>\n");
  fprintf(fp, "rencia<br>vs<br>meta<br>\n");
  fprintf(fp, "      </th>\n");
  fprintf(fp, "      <th style=\"vertical-align: center; text-align: center;\">Dif.<br>vs<br>Sal. Ant.\n");
  fprintf(fp, "      </th>");
  fprintf(fp, "      <th style=\"vertical-align: center; text-align: center;\">Tipo de <br>\n");
  fprintf(fp, "Interrup-<br>ci&oacute;n<br>\n");
  fprintf(fp, "      </th>\n");
  fprintf(fp, "    </tr>\n");
  fprintf(fp, "    <tr>\n");
/**********/
  char n_exit[50][200],int_type[50][200];
  int x;
  x  = get_int(dir,n_exit,int_type,200);
  int prog_idx = 0;
  int actual_idx = 0;
  time_t programmed_time;
  time_t actual_time; 
  int train_num;
  int total_dif = 0;
  int total_valid_dif = 0;
  int i,j,prev[2]={1000,1000};
  printf("ntimes %d\n",n_times);
  for (i=0,j=1; i < n_times; i++,j++)
  {
    int dif;
    programmed_time = programmed[prog_idx];
    train_num = train[prog_idx];
    prog_idx++;
    actual_time = actual[actual_idx];
    actual_idx++;
    int status;// = decide_status(programmed, prog_idx, actual, actual_idx, n_times);
    int filter;
    localtime_r(&programmed_time, &mytm);
    strftime(buff, sizeof(buff), "%T", &mytm);
    if(ln==1)
    {
     if(!strcmp(buff,"06:05:15"))
     {
      if(intervalcont[1]!=0)
      {
       interval[0]=interval[0]/intervalcont[1];
      }     
      intervalcont[0]=1;
      intervalcont[1]=0;
     }
     if(!strcmp(buff,"09:39:00"))
     {
      if(intervalcont[1]!=0)
      {
       interval[1]=interval[1]/intervalcont[1];
      }
      intervalcont[0]=2;
      intervalcont[1]=0;
     }
     if(!strcmp(buff,"12:10:00"))
     {
      if(intervalcont[1]!=0)
      {
       interval[2]=interval[2]/intervalcont[1];
      }
      intervalcont[0]=3;
      intervalcont[1]=0;
     }
     if(!strcmp(buff,"17:09:15"))
     {
      if(intervalcont[1]!=0)
      {
       interval[3]=interval[3]/intervalcont[1];
      }
      intervalcont[0]=4;
      intervalcont[1]=0;
     }
     if(!strcmp(buff,"20:44:30"))
     {
      if(intervalcont[1]!=0)
      {
       interval[4]=interval[4]/intervalcont[1];
      }
      intervalcont[0]=5;
      intervalcont[1]=0;
     }
    }
    if(!strcmp(buff,"22:32:00"))
    {
     if(intervalcont[1]!=0)
     {
      interval[5]=interval[5]/intervalcont[1];
     }
    }
    localtime_r(&actual_time, &mytm);
    strftime(buf2, sizeof(buf2), "%H", &mytm);
    if(prev[0]==1000)
    {
     prev[0]=0;
     prev[0]+=mytm.tm_sec;
     prev[0]+=mytm.tm_min*60;
     prev[0]+=mytm.tm_hour*60*60;
     if(prev[0]<17000)
	prev[0]=1000;
    }
    else
    {
     prev[1]=0;
     prev[1]+=mytm.tm_sec;
     prev[1]+=mytm.tm_min*60;
     prev[1]+=mytm.tm_hour*60*60;
    }
    filter=atoi(buf2);
    if(filter<5)
    {
     if(filter==4)
     {
      localtime_r(&actual_time, &mytm);
      strftime(buf2, sizeof(buf2), "%M", &mytm);
      filter=atoi(buf2);
      if(filter<30)
      {
       j--;
       i--;
       prog_idx--;
       continue;
      }
     }
     else
     {
      i--;
      j--;
      prog_idx--;
      continue;
     }
    }
    localtime_r(&actual_time, &mytm);
    strftime(buf2, sizeof(buf2), "%H", &mytm);
    filter = atoi(buf2);
    localtime_r(&programmed_time, &mytm);
    strftime(buf2, sizeof(buf2), "%H", &mytm);
    if((filter-atoi(buf2))>15)
    {
     i--;
     j--;
     prog_idx--;
     continue;
    }
    status = NORMAL_DEPARTURE;
    if ((actual_time != 0) && (programmed_time != 0))
    {
      dif = actual_time - programmed_time;
      if (dif > 160)
      {
        status = MISSING_DEPARTURE;
        printf("******** %d missing departure at %d %d\n", dif, actual_idx, prog_idx);
        total_departures++;
      }
      else if (dif < -117)
      {
        printf("----- %d extra departure at %d %d\n", dif, actual_idx, prog_idx);
        status = EXTRA_DEPARTURE;
        total_departures++;
        n_times++;
        if(n_times!=i)
         pdc++;
      }
      else
      {
        status = NORMAL_DEPARTURE;
        if(n_times!=i)
         pdc++;
        total_departures++;
      }
    }

    if (status == EXTRA_DEPARTURE)
    {
      // Extra departure
      prog_idx--;
      programmed_time = 0;
      train_num = 0;
    }
    else if (status == MISSING_DEPARTURE)
    {
      // Missing  departure
      dif = programmed_time - actual_time;
      actual_idx--;
      actual_time = 0;
    }
    if (programmed_time == 0)
    {
      snprintf(buf1, sizeof(buf2), "-");
    }
    else
    {
      localtime_r(&programmed_time, &mytm);
      strftime(buf1, sizeof(buf1), "%T", &mytm);
    }

    if (actual_time == 0)
    {
      snprintf(buf2, sizeof(buf2), "-"); 
    }
    else
    {
      localtime_r(&actual_time, &mytm);
      strftime(buf2, sizeof(buf2), "%T", &mytm);
    }

    if ((actual_time == 0) || (programmed_time == 0))
    {
      snprintf(buf3, sizeof(buf3), "-"); 
      dif = 0;
      prev[0]=prev[1]=1000;
    }
    else
    {
      dif = actual_time - programmed_time;
      tms[i]=(float)dif;
      bool is_neg = dif < 0;
      dif = abs(dif);
      if (is_neg)
      {
        snprintf(buf3, sizeof(buf3), "(%02d:%02d)", dif / 60, dif % 60); 
      }
      else
      {
        snprintf(buf3, sizeof(buf3), "%02d:%02d", dif / 60, dif % 60); 
      }
      if(n_times!=i)
      {
       total_dif += dif;
       total_valid_dif++;
      }
    }

    if (actual_time != 0)
    {
      time_t now = time(NULL);
      int time_since_depart = now - actual_time;
      int square = time_since_depart / 24; 
      if (square < 47)
      {
        td[ntd].above = 1;
        td[ntd].train_number = train_num;
        td[ntd].depart_time = actual_time;
        td[ntd].square = square;
        ntd++;
      }
    }

    fprintf(fp, "<tr>\n");
    fprintf(fp, "      <td style=\"vertical-align: top; text-align:center;\">");
    if(j<=total_departures)
      fprintf(fp,"<a href=\"#\" onclick=\"openWin('/cgi-bin/interrupt1?%d+%s+%s');return false;\" class=\"menu8\">%d</a>\n",j,dir,station,j);

    else
      fprintf(fp,"%d\n",j);
    fprintf(fp, "</td>\n");

    fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">");
    if (train_num > 0) fprintf(fp, "%02d", train_num);
    else fprintf(fp, "-");

    fprintf(fp, "</td>\n");
    fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">");
    fprintf(fp, "%s", buf1);    
    fprintf(fp, "</td>\n");
    fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">");
    fprintf(fp, "%s", buf2);
    fprintf(fp, "</td>\n");
    fprintf(fp, "      <td style=\"");
    if (dif > 30)
    {
      fprintf(fp, "background-color: rgb(255, 150, 150); "); 
      pdc--;
    }
    else if (dif > 25)
    {
      fprintf(fp, "background-color: rgb(255, 255, 150); "); 
    }

    fprintf(fp, "vertical-align: top; text-align: center;\">");

    fprintf(fp, "%s", buf3);
    fprintf(fp, "</td>\n");
    fprintf(fp, "    <td style=\"vertical_align: top; text-align: center;\">");
    if(prev[1]!=1000)
    {
     fprintf(fp, "%02d:%02d</td>\n",(prev[1]-prev[0])/60,(prev[1]-prev[0])%60);
     interval[intervalcont[0]]+=prev[1]-prev[0];
     intervalcont[1]++;
     prev[0]=prev[1];
    }
    else
    {
     fprintf(fp, "-</td>\n");
    }
    fprintf(fp, "    <td style=\"vertical-align: top; text-align: center;\">");
    interflag=-1;
    for(int y=0;y<x;y++)
     if (atoi(n_exit[y])==j)
     {
      interflag=y;
      break;
     }
     else
      interflag=-1;
    if (interflag!= -1) 
    {
      fprintf(fp,"<a href=\"#\" onclick=\"openWin('/cgi-bin/interdel?%d+%s+%s');return false;\" class=\"menu8\">%s</a>\n",j,dir,station,int_type[interflag]);
    }
    else 
    {
      fprintf(fp, "-");
    }
    fprintf(fp, "</td>\n");

    fprintf(fp, "    </tr>\n");
  }
  double avg_dif = (double) total_dif / ((double) total_valid_dif)-1;

  fprintf(fp, "    <tr>\n");
  fprintf(fp, "    <tr>\n");
  if((n_times-(int)total_departures)==1)
   total_departures++;
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: right;\"> %d\n",(int)total_departures);
  fprintf(fp, "      </td>\n");
  fprintf(fp, "      <td colspan=\"2\" style=\"vertical-align: top; text-align: right;\">Promedio \n");
  fprintf(fp, "      </td>\n");
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n", 
                        int(avg_dif) / 60, int(avg_dif) % 60);
  fprintf(fp, "      </td>\n");
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\"><br>\n");
  fprintf(fp, "      </td>\n");
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\"><br>\n");
  fprintf(fp, "      </td>\n");
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\"><br>\n");
  fprintf(fp, "      </td>\n");
  fprintf(fp, "</tr>\n");

/**************************************************************/
//Porciento de cumplimiento
  fprintf(fp, "    <tr>\n");
  fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\">Porciento de Cumplimiento\n(Salidas con una diferencia de +/- 30seg)\n");
  fprintf(fp, "      </td>\n");
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%.2f%%<br>\n",(pdc/total_departures)*100);
  fprintf(fp, "      </td>\n");
  fprintf(fp, "    </tr>\n");
/***************************************************************/
/***************************************************************/
//Sumatoria
  fprintf(fp, "    <tr>\n");
  fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\">Sumatoria de dif. de tiempos\n");
  fprintf(fp, "      </td>\n");
  hr_dif=(total_dif / 60) / 60;
  min_dif=(total_dif / 60);
  if(hr_dif>0)
   min_dif= min_dif - hr_dif*60;
  sec_dif = total_dif - ((hr_dif*60*60)+min_dif*60);
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d:%02d<br>\n",
 hr_dif, min_dif, sec_dif);
  fprintf(fp, "      </td>\n");
  fprintf(fp, "    </tr>\n");
/***************************************************************/
/***************************************************************/
//Desviación estándar
  fprintf(fp, "    <tr>\n");
  fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\">Desviaci&oacute;n Est&aacute;ndar\n");
  fprintf(fp, "      </td>\n");
  sum=0;
  r_prom=0;
  for(int g=0;g<total_departures-2;g++)
  {
   if((int)tms[g]>-117&&(int)tms[g]<160)
    r_prom +=tms[g];
  }
  r_prom = r_prom/total_departures;
  for(int g=0;g<total_departures-2;g++)
  {
   if((int)tms[g]>-117&&(int)tms[g]<160)
    sum += pow((tms[g]-r_prom),2);
   else
    sum += pow((0-r_prom),2);
  }
  des = sqrt((sum/(total_departures-2)));
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%.1lfseg<br>\n",des);
  fprintf(fp, "      </td>\n");
  fprintf(fp, "    </tr>\n");

/***************************************************************/
/***************************************************************/
//Rango
  fprintf(fp, "    <tr>\n");
  fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\">Rango\n");
  fprintf(fp, "      </td>\n");
  pos=tms[0];
  neg=tms[0];
  for(int g=0;g<total_departures-2;g++)
  {
   if((int)tms[g]>-360&&(int)tms[g]<360)
   {
    if(tms[g]>pos)
      pos=tms[g];
    if(tms[g]<neg)
      neg=tms[g];
   }
  }
  fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(pos-neg)/60),(int)(pos-neg)%60);
  fprintf(fp, "      </td>\n");
  fprintf(fp, "    </tr>\n");
/***************************************************************/
/***************************************************************/
//Intervalos sur
  if(ln==1)
  {
   fprintf(fp, "    <tr>\n");
   fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\"><small>Diferencia Promedio Intervalo 05:00:00 - 06:00:00</small>\n");
   fprintf(fp, "      </td>\n");
   fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(interval[0])/60),(int)(interval[0])%60);
   fprintf(fp, "      </td>\n");
   fprintf(fp, "    </tr>\n");
   fprintf(fp, "    <tr>\n");
   fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\"><small>Diferencia Promedio Intervalo 06:00:00 - 09:30:00</small>\n");
   fprintf(fp, "      </td>\n");
   fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(interval[1])/60),(int)(interval[1])%60);
   fprintf(fp, "      </td>\n");
   fprintf(fp, "    </tr>\n");
   fprintf(fp, "    <tr>\n");
   fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\"><small>Diferencia Promedio Intervalo 09:30:00 - 12:03:00</small>\n");
   fprintf(fp, "      </td>\n");
   fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(interval[2])/60),(int)(interval[2])%60);
   fprintf(fp, "      </td>\n");
   fprintf(fp, "    </tr>\n");
   fprintf(fp, "    <tr>\n");
   fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\"><small>Diferencia Promedio Intervalo 12:03:00 - 17:04:00</small>\n");
   fprintf(fp, "      </td>\n");
   fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(interval[3])/60),(int)(interval[3])%60);
   fprintf(fp, "      </td>\n");
   fprintf(fp, "    </tr>\n");
   fprintf(fp, "    <tr>\n");
   fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\"><small>Diferencia Promedio Intervalo 17:04:00 - 20:34:00</small>\n");
   fprintf(fp, "      </td>\n");
   fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(interval[4])/60),(int)(interval[4])%60);
   fprintf(fp, "      </td>\n");
   fprintf(fp, "    </tr>\n");
   fprintf(fp, "    <tr>\n");
   fprintf(fp, "      <td colspan=\"6\" style=\"vertical-align: top; text-align: right;\"><small>Diferencia Promedio Intervalo 20:34:00 - 22:32:00</small>\n");
   fprintf(fp, "      </td>\n");
   fprintf(fp, "      <td style=\"vertical-align: top; text-align: center;\">%02d:%02d<br>\n",((int)(interval[5])/60),(int)(interval[5])%60);
   fprintf(fp, "      </td>\n");
   fprintf(fp, "    </tr>\n");
  }
/***************************************************************/


  time_t gtim = time(NULL);
  struct tm gtm;
  FILE *pdc_h;
  char path[200];/*="/var/www/html/react/log/";
  int l=strlen(path);
  l+=strlen(dir);*/
  localtime_r(&gtim,&gtm);
  strftime(D,sizeof(D),"%d",&gtm);
/*  strncat(path,dir,l);
  strncat(path,"pdc.txt",l+7);*/
  snprintf(path,sizeof(path),"/var/www/html/react/log/%spdc.txt",dir);
  printf("%s\n",path);
  remove(path);
  pdc_h = fopen(path,"w+");
  if (pdc_h == NULL)
  {
    printf("%s:%d: ",__FILE__,__LINE__);
    perror(path);
    exit(1);
  }
  fprintf(pdc_h,"%s\t%.2f\n",D,(pdc/total_departures)*100);
  fclose(pdc_h);
/***************************************************************/
  FILE *gra;
  char day[50][100],pc[50][100],c;
  int c1=0,c2=0,f=0,jk;
  //strcpy(path,"/var/www/html/react/log/");
  gtim = time(NULL);
  localtime_r(&gtim,&gtm);
  strftime(M,sizeof(M),"%b",&gtm);
  strftime(D,sizeof(D),"%d",&gtm);
  /*l=strlen(path);
  l+=strlen(M);
  strncat(path,M,l);
  l=strlen(path);
  l+=strlen(dir);
  strncat(path,dir,l);
  strncat(path,".txt",l+4);*/
  snprintf(path,sizeof(path),"/var/www/html/react/log/%s%s.txt",M,dir);
  printf("%s\n",path);
  gra = fopen(path,"r");
  if(gra!=NULL)
  {
   while(!feof(gra))
   {
    fread(&c,sizeof(c),1,gra);
    if(!isspace(c))
    {
     if(f==0)
      day[c1][c2]=c;
     else
      pc[c1][c2]=c;
     c2++;
    }
    else
    {
     if(f==0)
     {
      day[c1][c2]='\x0';
      f=1;
      c2=0;
     }
     else
     {
      pc[c1][c2]='\x0';
      f=0;
      c1++;
      c2=0;
     }
     c='\x0';
    }
   }
   fclose(gra);
  }
  else
  {
   gra = fopen(path,"w+");
   fprintf(gra,"%s\t%.2f\n",D,(pdc/total_departures)*100);
   //strncpy(day[c1],D,strlen(D));
   snprintf(day[c1],sizeof(day[c1]),"%s",D);
   c1++;
   fclose(gra);
  }
  for(jk=0;jk<c1;jk++)
  {
   if(strcmp(day[jk],D)==0)
   {
    snprintf(pc[jk],sizeof(pc[jk]),"%.2f",(pdc/total_departures)*100);
    break;
   }
  }
  if(jk==c1)
  {
   //strncpy(day[c1],D,strlen(D));
   snprintf(day[c1],sizeof(day[c1]),"%s",D);
   snprintf(pc[c1],sizeof(pc[c1]),"%.2f",(pdc/total_departures)*100);
   c1++;
  }
  gra = fopen(path,"w+");
   for(jk = 0;jk < c1; jk ++)
   {
    fprintf(gra,"%s\t%s\n",day[jk],pc[jk]);
   }
  fclose(gra);
  fprintf(fp, "  </tbody>\n");
  fprintf(fp, "</table>\n");
  fprintf(fp, "</center>\n");
  fprintf(fp, "<br>\n");
  fprintf(fp, "<br>\n");
  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");
}

//#define HOME_DIR "/var/www/html/react/log"
#define HOME_DIR "../public_html/log"

int main(int argc, char *argv[])
{
  int train[500];
  time_t actual[500];
  time_t programmed[500];
  int interuption[500];
  struct tm mytm;
  time_t mytime;

  for (int i=0; i < 500; i++)
  {
    train[i] = 0;
    programmed[i] = 0;
    actual[i] = 0;
  }

  if (argc < 3)
  {
     printf("Not enough args\n");
     exit(0);
  }
  char date_str[50];
  if (argc > 3)
  {
    //strcpy(date_str, argv[3]);
    snprintf(date_str,sizeof(date_str),"%s",argv[3]);
  }
  else
  {
    date_str[0] = '\0';
  }

  mytime = time(NULL);
  localtime_r(&mytime, &mytm);
  if (date_str[0] == '\0')
  {
    strftime(date_str, sizeof(date_str), "%Y%m%d", &mytm);
  }
  if (NULL == strptime(date_str, "%Y%m%d", &mytm))
  {
    printf("Can't convert date: %s\n", date_str); 
    exit(0);
  }

  mytime = mktime(&mytm);

  char fname[200];
  snprintf(fname, sizeof(fname),
         "%s/%s/%s_log.txt", HOME_DIR, argv[1], date_str);
  int week_day = mytm.tm_wday;
  FILE *hd;
  hd=fopen("/var/www/html/react/log/festivo.txt","r");
  int month_day = mytm.tm_mday;
  int month = mytm.tm_mon;
  const char *sched_type;
  char sched_fname[200];
  switch (week_day)
  {
    case 6:
      sched_type = "sabado.txt";
      break; 
    case 0:
      sched_type = "domingo.txt";
      break; 
    default:
      sched_type = "laboral.txt";
      break; 
  }
  if(hd==NULL)
   printf("Cannot read the file festivo.txt");
  else
  {
   int cont=0,cont2=0,flag=0;
   char month_fest[20][10],day_fest[30][30],c;
   while(!feof(hd))
   {
    fread(&c,sizeof(c),1,hd);
    if(!isspace(c))
    {
     if(flag)
      day_fest[cont][cont2]=c;
     else
      month_fest[cont][cont2]=c;
     cont2++;
    }
    else
    {
     if(flag)
     {
      flag=0;
      day_fest[cont][cont2]='\x0';
      cont++;
     }
     else
     {
      flag=1;
      month_fest[cont][cont2]='\x0';
     }
     cont2=0;
    }
   }
   fclose(hd);
   for(cont2=0;cont2<cont;cont2++)
   {
    if(month==(atoi(month_fest[cont2])-1))
     if(month_day==atoi(day_fest[cont2]))
      sched_type="domingo.txt";
   }
  }
/***********************************************************/

  snprintf(sched_fname, sizeof(sched_fname), "%s/%s/%s", 
                  HOME_DIR, argv[1], sched_type);
  printf("Generating table for %s\n", date_str);
  printf("Day = %d, file = %s\n", week_day, sched_fname); 


  int n_sched = read_schedule(sched_fname, train, programmed, 500, mytime);
  for (int i=0; i < n_sched; i++)
  {
    char buf1[30];
    localtime_r(&programmed[i], &mytm);
    strftime(buf1, sizeof(buf1), "%T, %F", &mytm);
    /**
    int dif = 5;
    if (i == 5) dif = 10;
    if (i == 10) dif = 20;
    **/
    actual[i] = 0; 
  }

 
  int n_actual = n_read_actual(fname, actual, 500, mytime, 51);
  for (int i=0; i < n_actual; i++)
  {
    char buf1[30];
    localtime_r(&actual[i], &mytm);
    strftime(buf1, sizeof(buf1), "%T, %F", &mytm);
    if (i >= n_sched)
    {
      programmed[i] = 0;
      train[i] = 0;
    }
    strftime(buf1, sizeof(buf1), "%T, %F", &mytm);
  }
  int max = n_sched;
  printf("Scheduled: %d, Actual %d\n", n_sched, n_actual);
  char terminal[30];
  if(strcmp("periferico_sur",argv[1])==0)
  {
//    strcpy(terminal,"p_sur");
    snprintf(terminal,sizeof(terminal),"p_sur");
  }
  else
  {
    if(strcmp("periferico_norte",argv[1])==0)
    {
//      strcpy(terminal,"p_norte");
      snprintf(terminal,sizeof(terminal),"p_norte");
    }
    else
    {
//      strcpy(terminal,argv[1]);
      snprintf(terminal,sizeof(terminal),"%s",argv[1]);
    }
  }
  snprintf(fname, sizeof(fname),"%s/%s/%s_report_%s.html", HOME_DIR, argv[1], date_str,terminal);
  printf("Generating: %s\n", fname);
  FILE *fp;
  if ((fp = fopen(fname, "w")) == NULL)
  {
    printf("%s:%d: ",__FILE__,__LINE__);
    perror(fname);
    exit(0);
  }
  print_it(fp, train, programmed, actual, interuption, max, argv[2],week_day,argv[1]);
  fclose(fp);

  snprintf(fname, sizeof(fname),"%s/%s/display.html", HOME_DIR, argv[1]);
  printf("Generating: fname =>%s\n", fname);
  fp = fopen(fname, "w");
  if (fp == NULL)
  {
    printf("%s:%d: ",__FILE__,__LINE__);
    perror(fname);
    exit(0);
  }

  const char *background = "../Linea1Sur.png";
  const char *iname = "../square.png";
  int top = 50;
  int left = 0;

  gen_display(fp, background, top, left, iname, td, ntd);
}
