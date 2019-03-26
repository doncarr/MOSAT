/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/


/************************************************************************

pci9111.cpp

Contains code for pci9111 io card

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for open(...)
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "dask.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "pci9111.h"

/***********************************************************************/

void PCI9111_driver_t::read_voltage_file()
{
  /* ok, set the defaults first */
  for (int i=0; i < Max_Chan_9111; i++)
  {
    ranges[i] = AD_B_10_V;
    max_voltages[i] = 10.0;
  }
  /* now, open the file, if it exists, and set custom ranges */
  FILE *fp = fopen("dbfiles/pci9111.dat", "r");
  if (fp == NULL)
  {
    printf("Can't open dbfiles/pci9111.dat, using defaults\n");
    return;
  }

/**
#define AD_B_10_V       1
#define AD_B_5_V        2
#define AD_B_2_5_V      3
#define AD_B_1_25_V     4
#define AD_B_0_625_V    5
#define AD_B_0_3125_V   6
#define AD_B_0_5_V      7
#define AD_B_0_05_V     8
#define AD_B_0_005_V    9
#define AD_B_1_V       10
#define AD_B_0_1_V     11
#define AD_B_0_01_V    12
#define AD_B_0_001_V   13
#define AD_U_20_V      14
#define AD_U_10_V      15
#define AD_U_5_V       16
#define AD_U_2_5_V     17
#define AD_U_1_25_V    18
#define AD_U_1_V       19
#define AD_U_0_1_V     20
#define AD_U_0_01_V    21
#define AD_U_0_001_V   22
**/

  char line[100];
  for (int i=0; NULL != fgets(line, sizeof(line) , fp); i++)
  {
    ltrim(line);
    rtrim(line);

    if (0 == strcmp(line, "10"))
    {
      ranges[i] = AD_B_10_V;
      max_voltages[i] = 10.0;
      printf("pci9111: ch%d 10V\n", i);
    }
    else if (0 == strcmp(line, "5"))
    {
      ranges[i] = AD_B_5_V;
      max_voltages[i] = 5.0;
      printf("pci9111: ch%d 5V\n", i);
    }
    else if (0 == strcmp(line, "2.5"))
    {
      ranges[i] = AD_B_2_5_V;
      max_voltages[i] = 2.5;
      printf("pci9111: ch%d 2.5V\n", i);
    }
    else if (0 == strcmp(line, "1.25"))
    {
      ranges[i] = AD_B_1_25_V;
      max_voltages[i] = 1.25;
      printf("pci9111: ch%d 1.25V\n", i);
    }
    else
    {
      printf("Bad line in dbfiles/pci9111.dat: %s\n", line);
    }
  }
}

/***********************************************************************/

extern "C" io_driver_t *new_pci9111(react_drv_base_t *r, const char *option)
{
  printf("Creating new pci9111 iodriver\n");
  return new PCI9111_driver_t(r);
}


/***********************************************************************/

PCI9111_driver_t::PCI9111_driver_t(react_drv_base_t *react)
{
  driver_name = "PCI9111";
  driver = 1;
  card = 1;

  //#define Max_Chan_9111 (8)
  //I16 cardnum;
  //I16 chan_data[Max_Chan];
  //F32 chan_voltage[Max_Chan];
  //I16 range=AD_B_10_V;

  cardnum = Register_Card(PCI_9111DG, 0);
  if (cardnum != 0)
  {
     printf("Register_Card error=%d\n", cardnum);
     exit(1);
  }

  read_voltage_file();

  /* Initialize to Bibolar -10 to 10 */
  U16 output_type = P9111_AO_BIPOLAR;
  int retv = AO_9111_Config (cardnum, output_type);
  if (retv != 0)
  {
    printf("AO_9111_Config error=%d\n", retv);
    exit(1);
  }

  retv = AO_VWriteChannel(cardnum, 0, 0.0);
  if (retv != 0)
  {
    printf("AO_VWrite error=%d, card = %hu\n", retv, card);
    exit(1);
  }
  I16 err;
  err = CTR_Clear(cardnum, 0, 0);
  if (err != NoError)
  {
    printf(" CTR_Clear error : error_code: %d \n", err );
  }

  dobuf = 0;
  printf("PCI9111 initialized\n", retv);
}

/***********************************************************************/

void PCI9111_driver_t::send_ao(int ch, double val)
{
  //printf("Send AO ch = %d, val = %lf\n", ch, val);
  if (ch == 0)
  {
    int retv = AO_VWriteChannel(cardnum, 0, val);
    //printf("Send AO: %d, %lf, %d\n", ch, val, retv);
  }
  else
  {
    if (val < 0.0)  val = 0.0;
    unsigned long ival = (unsigned long) (val + 0.5);
    if (ival > 0xFF) ival = 0xFF;
    //printf("Before: %lx\n", dobuf);
    if (ch == 1)
    {
      // here we will use the first 8 bit port for an AO.
      dobuf &= 0xFF00;
      //printf("Clear: %lx\n", dobuf);
      dobuf |= ival;
      //printf("After: %lx\n", dobuf);
    }
    else if (ch == 2)
    {
      // here we will use the second 8 bit port for an AO.
      dobuf &= 0x00FF;
      //printf("Clear: %lx\n", dobuf);
      dobuf |= ival << 8;
      //printf("After: %lx\n", dobuf);
    }
    DO_WritePort(cardnum,0, dobuf);
  }
}

/***********************************************************************/

void PCI9111_driver_t::close(void)
{
  Release_Card(cardnum);
}

/***********************************************************************/

void PCI9111_driver_t::send_do(int channel, bool val)
{
  U32 mask = ((U32) 1) << channel;
  if (val)
  {
    dobuf |= mask;
  }
  else
  {
    dobuf &= ~mask;
  }
  DO_WritePort(cardnum,0, dobuf);
  //printf("        Value write to OUTPUT PORT : %02x                        \n\n", (U16)dobuf);
     //DI_ReadPort(card,0,&input);
     //printf("        Value read from INPUT PORT : %02x                        \n\n\n\n\n\n\n", (U16)input );


   //  usleep( 10000 );
    // if( output_value == 0xff ) output_value = 0;
     //else output_value ++;

  //}while( !kb_hit() );

  //Release_Card( card );
}

/***********************************************************************/

double PCI9111_driver_t::get_ai(int channel)
{
  if ((channel >= 0) && (channel < 8))
  {
    return chan_voltage[channel];
  }
  else if (channel == 9)
  {
    return (double) di_input_reg;
  }
  else
  {
    return 0.0;
  }
}

/***********************************************************************/

bool PCI9111_driver_t::get_di(int channel)
{
  if ((channel >= 0) && (channel < 8))
  {
    return di_data[channel];
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

long PCI9111_driver_t::get_count(int channel)
{
  if ((channel >= 0) && (channel < 1))
  {
    return chan_count[channel];
  }
  else
  {
    return 0;
  }
}

/***********************************************************************/

void PCI9111_driver_t::read(void)
{
  U32 mask;
  I16 err;
  DI_ReadPort(cardnum,0,&di_input_reg);
  //printf("Value read from INPUT PORT : %02x\n", (U16)di_input_reg );
  for (int i=0; i < 8; i++)
  {
    mask = ((U32) 1) << i;
    di_data[i] = (di_input_reg & mask) != 0;
  }
  for(int i=0; i < Max_Chan_9111; i++)
  {
    /***
    if((err = AI_ReadChannel(cardnum, i, ranges[i], (U16*)&chan_data[i]) )
		    != NoError )
    ***/
    if((err = AI_VReadChannel(cardnum, i, ranges[i], &chan_voltage[i]) )
		    != NoError )

    {
      printf(" AI_VReadChannel Ch#%d error : error_code: %d \n", i, err );
      chan_data[i] = 0;
      chan_voltage[i] = 0;
    }
    else
    {
      //printf("raw = %03x\n", chan_data[i]);
      //chan_data[i] = chan_data[i] >> 4;
      chan_data[i] = 0;
      //chan_voltage[i] = (F32)( chan_data[i]* max_voltages[i] )/ 2047;
      //chan_data[i] = chan_data[i] & 0x0fff;
    }
   }
   err = CTR_Read(cardnum, 0, &chan_count[0]);
   if (err != NoError)
   {
     printf(" CTR_Read error : error_code: %d \n", err );
     chan_count[0] = 0;
   }
   //printf("                Ch0       Ch1       Ch2       Ch3\n");
   //printf(" input value :  %03x       %03x       %03x       %03x\n", \
         chan_data[0], chan_data[1], chan_data[2], chan_data[3]);
   //printf("     voltage :  %.2f      %.2f      %.2f      %.2f\n\n", \
       chan_voltage[0], chan_voltage[1], chan_voltage[2], chan_voltage[3]);
}

/***********************************************************************/

/*
#include <stdio.h>
#include <fcntl.h> // for open(...)
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "conio.h"

#define Max_Chan 4


int main( void )
{
  int i;
  I16 card, err;
  I16 range=AD_B_10_V;
  char temp_char;
  I16 chan_data[Max_Chan];
  F32 chan_voltage[Max_Chan],
  max_voltage=10;

  setbuf( stdout, NULL );
  if ((card=Register_Card (PCI_9111DG, 0)) <0 ) {
     printf("Register_Card error=%d\n", card);
     exit(1);
  }

  clrscr();
  printf("*****************************************************************" );
  printf("                      PCI9111 AI Polling                         \n");
  printf("*****************************************************************\n\n");
  printf(" please select the AI input range :                              \n");
  printf(" (1)AD_B_10_V    (2)AD_B_5_V     (3)AD_B_2_5_V  :");
  temp_char = getch();
  if(  temp_char < 0x31 || temp_char > 0x33 ){
      range = AD_B_5_V; max_voltage = 5;
      printf(" AI input range uncorrect , use the default value 'AD_B_5_V'.\n");
      printf("                                    pesss any key to continue\n");
      getch();
  }else {
      range = temp_char - 0x30;
      max_voltage = (F32)10 / ( 1<<(range-1) );
  }

  do{
     clrscr();
     printf("*****************************************************************\n" );
     printf("                      PCI9111 AI Polling                         \n");
     printf("*****************************************************************\n\n");
     printf("                                                                 \n\n\n\n");
     for( i=0 ; i<Max_Chan; i++ ){
         if( (err = AI_ReadChannel(card, i, range, (U16*)&chan_data[i]) ) != NoError )
             printf(" AI_ReadChannel Ch#%d error : error_code: %d \n", i, err );
         chan_data[i] = chan_data[i] >> 4;
         chan_voltage[i] = (F32)( chan_data[i]* max_voltage )/ 2047;
         chan_data[i] = chan_data[i] & 0x0fff;
     }
     printf("                Ch0       Ch1       Ch2       Ch3\n");
     printf(" input value :  %03x       %03x       %03x       %03x\n", \
                             chan_data[0], chan_data[1], chan_data[2], chan_data[3]);
     printf("     voltage :  %.2f      %.2f      %.2f      %.2f\n\n", \
                             chan_voltage[0], chan_voltage[1], chan_voltage[2], chan_voltage[3]);
     printf("\n\n\n\n\n                                       press any key to stop \n");
     usleep( 50000 );

  }while(!kb_hit() );

  Release_Card( card );

  return 0;
}

***/





