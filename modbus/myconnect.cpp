
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include "mtype.h"
//#include "modio.h"
//#include "hprtu.h"
//#include "hp2mod.h"
#include "rtmodbus.h"

#define tp_serial (1)
#define tp_tcpip (2)

void my_sighandler(int signum)
{
  //printf("Got signal\n");
}

// ssh dcarr@coyote2.dyndns.org:52483
int main(int argc, char *argv[])
{
  const char *ip_addr;
  const char *serial_dev;
  float timeout;
  int baudrate;
  int connection_type;
 
  int unit_id;

  signal(SIGPIPE, SIG_IGN);
  signal(SIGIO, SIG_IGN);
  //signal(SIGIO , my_sighandler);

  if (argc == 5)
  {
    connection_type = tp_serial;
    serial_dev = argv[1];
    baudrate = atoi(argv[2]);
    timeout = atof(argv[3]); 
    unit_id = atoi(argv[4]);
  }
  else
  {
    connection_type = tp_tcpip;
    if (argc > 1)
    {
      ip_addr = argv[1];
      printf("Using: %s\n", ip_addr);
    }
    else
    {
      ip_addr = "127.0.0.1:502";
      printf("Using default: %s\n", ip_addr); 
    }

    if (argc > 2)
    {
      unit_id = atol(argv[2]);
      printf("Using unit id: %d\n", unit_id);
    }
    else
    {
      unit_id = 0;
      printf("Using default unit id:: %d\n", unit_id);
    }
  }


  //printf("myconnect %s:%d\n", __FILE__, __LINE__);
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.32:502");
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.27:502");
  //rtmodbus_t *modc = rt_create_modbus("192.168.0.100");
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.99");
  //rtmodbus_t *modc = rt_create_modbus("192.168.1.104");
  //rtmodbus_t *modc = rt_create_modbus("coyote2.dyndns.org:502");
  //rtmodbus_t *modc = rt_create_modbus("189.162.54.98:502");
  	//rtmodbus_t *modc = rt_create_modbus("172.16.115.32");
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.35");
  //rtmodbus_t *modc = rt_create_modbus("172.16.115.37");
  //rtmodbus_t *modc = rt_create_modbus("10.1.0.5");
  //rtmodbus_t *modc = rt_create_modbus("10.0.0.0");

  rtmodbus_t *modc;
  if (connection_type == tp_tcpip)
  {
    modc = rt_create_modbus(ip_addr);
    printf("myconnect %s:%d\n", __FILE__, __LINE__);
  }
  else
  {
    modc = rt_create_modbus_serial(serial_dev, baudrate, timeout, 0);
  }
  modc->set_debug_level(10);
  modc->set_address(unit_id);
 
  while (true)
  {
    printf("1 (opcode 0x01) - read discrete outputs\n");
    printf("2 (opcode 0x02) - read discrete inputs\n");
    printf("3 (opcode 0x03) - read registers\n");
    printf("4 (opcode 0x04) - read analog inputs\n");
    printf("5 (opcode 0x05) - send DO\n");
    printf("6 (opcode 0x06) - write registers\n");
    printf("7 (opcode 0x03) - read DIs in register\n");
    printf("8 (opcode 0x0F) - write multiple discrete outputs\n");
    printf("9 (opcode 0x10) - write multiple registers\n");
    printf("10 (-) - Set modbus address\n");
    printf("99 (-)- Exit\n");
    printf("Enter your choice: ");
    fflush(stdin);
    int option;
    scanf("%d", &option);
    fflush(stdin);
    /*fgets(mybuf, sizeof(mybuf), stdin);
    if (0 == strncmp(mybuf, "quit", 4))
    {
      //shutdown(sockfd, 5);
      exit(0);
    }
    */
    if (99 == option)
    {
      exit(0);
    }
    unsigned short uvals[20];
    bool vals[50];
    unsigned char bvals[50];
    int start;
    int num;
    int bval;
    switch (option)
    {
      case 5:
	printf("Enter the do number: ");
	scanf("%d", &start);
	printf("Enter the value (0/1): ");
	scanf("%d", &bval);
        modc->send_do(start, (bval != 0));
        break;
      case 2:
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
      case 4:
	printf("Enter the start ai: ");
	scanf("%d", &start);
        modc->read_ai(start, 16, uvals); 
        for (int i=0; i < 16; i++)
        {
          printf("%d ", (int) uvals[i]);
        }
        printf("\n");
        for (int i=0; i < 2; i++)
        {
          float f;
          char * p = (char *) &f;
          memcpy(p, &uvals[2*i] ,2);
          memcpy(p+2, &uvals[(2*i)+1] ,2);
          printf("%d: %f\t", i, f);
          memcpy(p+2, &uvals[2*i] ,2);
          memcpy(p, &uvals[(2*i)+1] ,2);
          printf("%d: %f\t", i, f);
        }
        printf("\n");
        break;
      case 1:
	printf("Enter the start do: ");
	scanf("%d", &start);
        modc->read_do(start, 16, vals); 
        for (int i=0; i < 16; i++)
        {
          printf("%c ", vals[i] ? '1' : '0');
        }
        printf("\n");
	break;
      case 3:
	printf("Enter the start reg: ");
	scanf("%d", &start);
        modc->read_reg(start, 16, uvals); 
        for (int i=0; i < 16; i++)
        {
          printf("%d ", (int) uvals[i]);
        }
        printf("\n");
        break;
      case 6:
	printf("Enter the register num: ");
	scanf("%d", &start);
	printf("Enter the register value: ");
	scanf("%hu", &uvals[0]);
        modc->write_reg(start, uvals[0]); 
        printf("wrote %d to reg %d\n", (int) uvals[0], (int) start);
        break;
      case 7:
	printf("Enter the start register: ");
	scanf("%d", &start);
        printf("Reading 16 DIs at %d\n", start);
        modc->read_di_register(start, 16, vals); 
        for (int i=0; i < 16; i++)
        {
          printf("%c ", vals[i] ? '1' : '0');
        }
        printf("\n");
        break;
      case 9:
	printf("Enter the start register to write: ");
	scanf("%d", &start);
	printf("How many registers to write? (max 8): ");
	scanf("%d", &num);
        if (num > 8) num = 8;
        if (num < 1) num = 1;
        for (int i=0; i < num; i++)
        {
	  printf("Value for register %d: ", i+1);
	  scanf("%hu", &uvals[i]);
        }
        modc->write_multiple_regs(start, num,  uvals);
        break;
      case 8:
	printf("Enter the start discrete out to write: ");
	scanf("%d", &start);
	printf("How many discrete outs to write? (max 8): ");
	scanf("%d", &num);
        if (num > 8) num = 8;
        if (num < 1) num = 1;
        for (int i=0; i < num; i++)
        {
	  printf("Value for discrete out %d: ", i+1);
	  scanf("%d", &bval);
          bvals[i] = bval != 0;
        }
        modc->send_multiple_dos(start, num,  bvals);
        break;
      case 10:
	printf("Enter the modbus address: ");
	scanf("%d", &start);
        modc->set_address(start); 
        break;
    }
  }
}
