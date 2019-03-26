
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "arg.h"

//#define DEST_IP   "127.0.0.1"
//#define DEST_IP   "216.109.112.135"
//#define DEST_IP   "172.16.115.36"

#define DEST_PORT 3489 
//#define DEST_PORT 80 

void gen_log(char newhour[],char actualhour[],char dir[])
{
 time_t t;
 struct tm ttm;
 t=time(NULL);
 localtime_r(&t,&ttm);
 char date[50];
 strftime(date,sizeof(date),"%d/%m/%Y",&ttm);
 printf("Fecha: %s\n",date);
 char path[200];
 strcpy(path,dir);
 strcat(path,"/sync_log.txt");
 printf("Path = \"%s\"\n",path);
 FILE *fp;
 fp=fopen(path,"a+");
 if(fp!=NULL)
 {
  fprintf(fp,"%s\tActual: %s\tNueva: %s\n",date,actualhour,newhour);
  printf("Log file generado!\n");
  fclose(fp);
 }
 else
 {
  printf("No se puede leer el archivo\n");
 }
}

int get_ip_address(char ip[10][100],char dir[10][100])
{
 FILE *fd;
 int cont = 0;
 char line[300];
 char *args[10];
 fd = fopen("/home/react/modbus/sync_list.txt","r");
 if(fd!=NULL)
 {
  clearerr(fd);
  while(!feof(fd))
  {
   fgets(line,sizeof(line),fd);
   if(line[0]=='#')
    continue;
   get_delim_args(line,args,'|',2);
   strcpy(ip[cont],args[0]);//args[0],strlen(args[0])+1);
   strcpy(dir[cont],args[1]);
   cont++;
  }
  fclose(fd);
 }
 else
 {
  puts("el archivo no se puede leer\n");
 }
 return cont;
}

int main()
{
  int direc;
  char ip[10][100],dir[10][100];
  direc = get_ip_address(ip,dir);
  for(int zz = 0;zz <direc; zz++)
  {
   printf("main dir[%i] = %s\n",zz,dir[zz]);
   printf("main ip[%i] = %s\n",zz,ip[zz]);
  }
  for (int a = 0;a<direc;a++)
  {
    int sockfd;
    struct sockaddr_in dest_addr;   /* will hold the destination addr */

    sockfd = socket(AF_INET, SOCK_STREAM, 0); /* do some error checking! */
    dest_addr.sin_family = AF_INET;        /* host byte order */
    dest_addr.sin_port = htons(DEST_PORT); /* short, network byte order */
    dest_addr.sin_addr.s_addr = inet_addr(ip[a]);
    memset(&(dest_addr.sin_zero), 8, 0);       /* zero the rest of the struct */
     /* don't forget to error check the connect()! */
    connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
    char buf[300];
    time_t t;
    struct tm mytm; 
//  char buf[300];
    t = time(NULL);
    localtime_r (&t,&mytm);
    char mybuf[200];
    strftime(mybuf,sizeof(mybuf),"%H:%M:%S",&mytm);
    printf("%s", buf);
//    char mybuf[200];
//    printf("Enter a string: ");
//    fgets(mybuf, sizeof(mybuf), stdin);
/*    int xxx = read(sockfd, buf,sizeof(buf));
    if(xxx == -1) 
    {
      printf("Reading Error\n");
      continue;
    }*/
    printf("%i Sending this message: %s\n", a,mybuf);
    write(sockfd, mybuf, strlen(mybuf) + 1);
    read(sockfd, buf, sizeof(buf));
    printf("Got the message %s\n",buf);
    gen_log(mybuf,buf,dir[a]);
//    if (0 == strncasecmp(mybuf, "quit", 4))
//    {
//      break;
//    }  
  }
 return 0;
}
