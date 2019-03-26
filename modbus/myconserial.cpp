
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include "mtype.h"
//#include "modio.h"
//#include "hprtu.h"
//#include "hp2mod.h"
#include "rtmodbus.h"



int main(int argc, char *argv[])
{
  printf("myconnect %s:%d\n", __FILE__, __LINE__);
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.32:502");
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.27:502");
  //rtmodbus_t *modc = rt_create_modbus("127.0.0.1");
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.37");
  //rtmodbus_t *modc = rt_create_modbus("10.1.0.5");
  rtmodbus_t *modc = rt_create_modbus_serial("/dev/ttyUSB0", 9600, 2.0);
  modc->set_debug_level(0);
  modc->set_address(5);
  printf("myconnect %s:%d\n", __FILE__, __LINE__);
 
  while (true)
  {
   
    char mybuf[200];
    printf("1nm - send DO n = channel (0-7), m = value (0,1)\n");
    printf("2   - read discrete inputs and outputs\n");
    printf("3   - read analog input and counters\n");
    printf("4   - read discrete outputs\n");
    printf("5   - Set TPO\n");
    printf("6   - Set modbus address\n");
    printf("7   - read register\n");
    printf("8   - write register\n");
    printf("Enter your choice: ");
    fgets(mybuf, sizeof(mybuf), stdin);
    if (0 == strncmp(mybuf, "quit", 4))
    {
      //shutdown(sockfd, 5);
      exit(0);
    }
    unsigned short uvals[20];
    bool vals[50];
    int start;
    switch (mybuf[0])
    {
      case '1':
        //modc->send_do(mybuf[1]-'0', mybuf[2]-'0');
        modc->send_do(173,1);
        break;
      case '2':
	printf("Enter the start: ");
	scanf("%d", &start);
        printf("Reading 24 DIs at 0\n");
        modc->read_di(start, 24, vals); 
        for (int i=0; i < 24; i++)
        {
          printf("%c ", vals[i] ? '1' : '0');
        }
        printf("\n");
        break;
      case '3':
	printf("Enter the start ai: ");
	scanf("%d", &start);
        modc->read_ai(start, 16, uvals); 
        for (int i=0; i < 16; i++)
        {
          printf("%d ", (int) uvals[i]);
        }
        printf("\n");
        break;
      case '4':
        modc->read_do(0, 32, vals); 
        for (int i=0; i < 24; i++)
        {
          printf("%c ", vals[i] ? '1' : '0');
        }
        printf("\n");
	break;
      case '5':
	printf("Enter the TPO value: ");
	scanf("%d", &start);
        modc->write_reg(0, start); 
        break;
      case '6':
	printf("Enter the modbus address: ");
	scanf("%d", &start);
        modc->set_address(start); 
        break;
      case '7':
	printf("Enter the start reg: ");
	scanf("%d", &start);
        modc->read_reg(start, 16, uvals); 
        for (int i=0; i < 16; i++)
        {
          printf("%d ", (int) uvals[i]);
        }
        printf("\n");
        break;
      case '8':
	printf("Enter the register num: ");
	scanf("%d", &start);
	printf("Enter the register value: ");
	scanf("%hu", &uvals[0]);
        modc->write_reg(start, uvals[0]); 
        printf("wrote %d to reg %d\n", (int) uvals[0], (int) start);
        break;
    }
  }
}
