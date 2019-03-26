/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.5 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 06/12/2009
Author  : F4CG                            
Company : F4CG                            
Comments: 


Chip type           : ATmega2560
Program type        : Application
Clock frequency     : 16.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 2048
*****************************************************/

#include <mega2560.h>

// I2C Bus functions
#asm
   .equ __i2c_port=0x14 ;PORTG
   .equ __sda_bit=2
   .equ __scl_bit=0
#endasm
#include <i2c.h>

// PCF8583 Real Time Clock functions
#include <pcf8583.h>

#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 256
char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0<256
unsigned char rx_wr_index0,rx_rd_index0,rx_counter0;
#else
unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
#endif

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;

// USART0 Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
char status,data;
status=UCSR0A;
data=UDR0;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer0[rx_wr_index0]=data;
   if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
   if (++rx_counter0 == RX_BUFFER_SIZE0)
      {
      rx_counter0=0;
      rx_buffer_overflow0=1;
      };
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART0 Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter0==0);
data=rx_buffer0[rx_rd_index0];
if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
#asm("cli")
--rx_counter0;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART0 Transmitter buffer
#define TX_BUFFER_SIZE0 256
char tx_buffer0[TX_BUFFER_SIZE0];

#if TX_BUFFER_SIZE0<256
unsigned char tx_wr_index0,tx_rd_index0,tx_counter0;
#else
unsigned int tx_wr_index0,tx_rd_index0,tx_counter0;
#endif

// USART0 Transmitter interrupt service routine
interrupt [USART0_TXC] void usart0_tx_isr(void)
{
if (tx_counter0)
   {
   --tx_counter0;
   UDR0=tx_buffer0[tx_rd_index0];
   if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART0 Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter0 == TX_BUFFER_SIZE0);
#asm("cli")
if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer0[tx_wr_index0]=c;
   if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
   ++tx_counter0;
   }
else
   UDR0=c;
#asm("sei")
}
#pragma used-
#endif

// USART2 Receiver buffer
#define RX_BUFFER_SIZE2 64
char rx_buffer2[RX_BUFFER_SIZE2];

#if RX_BUFFER_SIZE2<256
unsigned char rx_wr_index2,rx_rd_index2,rx_counter2;
#else
unsigned int rx_wr_index2,rx_rd_index2,rx_counter2;
#endif

// This flag is set on USART2 Receiver buffer overflow
bit rx_buffer_overflow2;

// USART2 Receiver interrupt service routine
interrupt [USART2_RXC] void usart2_rx_isr(void)
{
char status,data;
status=UCSR2A;
data=UDR2;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer2[rx_wr_index2]=data;
   if (++rx_wr_index2 == RX_BUFFER_SIZE2) rx_wr_index2=0;
   if (++rx_counter2 == RX_BUFFER_SIZE2)
      {
      rx_counter2=0;
      rx_buffer_overflow2=1;
      };
   };
}

// Get a character from the USART2 Receiver buffer
#pragma used+
char getchar2(void)
{
char data;
while (rx_counter2==0);
data=rx_buffer2[rx_rd_index2];
if (++rx_rd_index2 == RX_BUFFER_SIZE2) rx_rd_index2=0;
#asm("cli")
--rx_counter2;
#asm("sei")
return data;
}
#pragma used-

// USART2 Transmitter buffer
#define TX_BUFFER_SIZE2 64
char tx_buffer2[TX_BUFFER_SIZE2];

#if TX_BUFFER_SIZE2<256
unsigned char tx_wr_index2,tx_rd_index2,tx_counter2;
#else
unsigned int tx_wr_index2,tx_rd_index2,tx_counter2;
#endif

// USART2 Transmitter interrupt service routine
interrupt [USART2_TXC] void usart2_tx_isr(void)
{
if (tx_counter2)
   {
   --tx_counter2;
   UDR2=tx_buffer2[tx_rd_index2];
   if (++tx_rd_index2 == TX_BUFFER_SIZE2) tx_rd_index2=0;
   };
}

// Write a character to the USART2 Transmitter buffer
#pragma used+
void putchar2(char c)
{
while (tx_counter2 == TX_BUFFER_SIZE2);
#asm("cli")
if (tx_counter2 || ((UCSR2A & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer2[tx_wr_index2]=c;
   if (++tx_wr_index2 == TX_BUFFER_SIZE2) tx_wr_index2=0;
   ++tx_counter2;
   }
else
   UDR2=c;
#asm("sei")
}
#pragma used-

// Standard Input/Output functions
#include <stdio.h>
#include <stdlib.h>
#include <delay.h>

unsigned int adc_data;
#define ADC_VREF_TYPE 0x00

// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void)
{
// Read the AD conversion result
adc_data=ADCW;
}

// Read the AD conversion result
// with noise canceling
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
ADCSRA&=0x7f;
ADCSRA|=0xc0;
#asm
    in   r30,smcr
    cbr  r30,__sm_mask
    sbr  r30,__se_bit
    out  smcr,r30
    sleep
    cbr  r30,__se_bit
    out  smcr,r30
#endasm
return adc_data;
}

// Declare your global variables here
#include "digital_channels.h"
#include "analog_channels.h"

#define FAN_PULSE 3            //seconds

#define SERIAL_TIMEOUT 60000   //count

eeprom int garbage;
eeprom unsigned char virtual_do[16];
eeprom unsigned int virtual_ao[16];

unsigned short int turn_on_state;
unsigned short int turn_off_state;

unsigned char rtc_h,rtc_m,rtc_s,rtc_hs;

unsigned short int tick;
unsigned short int seconds;
unsigned short int past_hs;

unsigned char ventilating;

unsigned int raw_temperature_general_e;
unsigned int raw_humidity_general_e;
unsigned int raw_humidity_honey_w;

unsigned int calculated_temperature_general_e;
unsigned int calculated_humidity_general_e;
unsigned int calculated_humidity_honey_w;


unsigned char read_bit(unsigned char bit_index, unsigned char value)
{
unsigned char temp;
unsigned char mask;

mask = 1 << bit_index;

temp = value & mask;
        if(temp == 0)
                return 0;
        else
                return 1;
}

unsigned char set_bit(unsigned char bit_index, unsigned char port, unsigned char value)
{
        unsigned char temp;
        unsigned char mask;
        
        mask = 1 << bit_index;
        
        if(value == 1)
        {
                temp = port | (mask);
        }
        else
        {
                mask = ~mask;
                temp = port & mask;
        }       
        return temp;
}

void a_rtc_set_time(unsigned short int value)
{
        unsigned char h,m;
        
        m = value % 60;
        h = value / 60;
        
        rtc_set_time(0, h, m, 0, 0);
        //printf("la hora: %u\n\r",h);
        //printf("Los minutos: %u\n\r",m);
         
}

unsigned short int a_rtc_get_time()
{
unsigned char rtc_h_temp, rtc_m_temp, rtc_s_temp, rtc_hs_temp;

        rtc_get_time(0,&rtc_h_temp, &rtc_m_temp, &rtc_s_temp, &rtc_hs_temp);
        
        return ((unsigned short int)rtc_h_temp*60) + (unsigned short int)rtc_m_temp;
}

void write_do(unsigned short int channel, unsigned short int value)
{
        if(channel == 0)
                if(value == 0)
                        PORTL = set_bit(7,PORTL,0);
                else
                        PORTL = set_bit(7,PORTL,1);
        else
        if(channel == 1)
                if(value == 0)
                        PORTL = set_bit(6,PORTL,0);
                else
                        PORTL = set_bit(6,PORTL,1);
        else
        if(channel == 2)
                if(value == 0)
                        PORTL = set_bit(5,PORTL,0);
                else
                        PORTL = set_bit(5,PORTL,1);
        else
        if(channel == 3)
                if(value == 0)
                        PORTL = set_bit(4,PORTL,0);
                else
                        PORTL = set_bit(4,PORTL,1);
        else
        if(channel == 4)
                if(value == 0)
                        PORTL = set_bit(3,PORTL,0);
                else
                        PORTL = set_bit(3,PORTL,1);
        else
        if(channel == 5)
                if(value == 0)
                        PORTL = set_bit(2,PORTL,0);
                else
                        PORTL = set_bit(2,PORTL,1);
        else
        if(channel == 6)
                if(value == 0)
                        PORTL = set_bit(1,PORTL,0);
                else
                        PORTL = set_bit(1,PORTL,1);
        else
        if(channel == 7)
                if(value == 0)
                        PORTL = set_bit(0,PORTL,0);
                else
                        PORTL = set_bit(0,PORTL,1);
        else
        if(channel == 8)
                if(value == 0)
                        PORTC.0 = 0;
                else
                        PORTC.0 = 1;
        else
        if(channel == 9)
                if(value == 0)
                        PORTC.1 = 0;
                else
                        PORTC.1 = 1;
        else
        if(channel == 10)
                if(value == 0)
                        PORTC.2 = 0;
                else
                        PORTC.2 = 1;
        else
        if(channel == 11)
                if(value == 0)
                        PORTC.3 = 0;
                else
                        PORTC.3 = 1;
        else
        if(channel == 12)
                if(value == 0)
                        PORTC.4 = 0;
                else
                        PORTC.4 = 1;
        else
        if(channel == 13)
                if(value == 0)
                        PORTC.5 = 0;
                else
                        PORTC.5 = 1;
        else
        if(channel == 14)
                if(value == 0)
                        PORTC.6 = 0;
                else
                        PORTC.6 = 1;
        else
        if(channel == 15)
                if(value == 0)
                        PORTC.7 = 0;
                else
                        PORTC.7 = 1;
        else 
        if(channel == 50)
                if(value == 0)
                        virtual_do[0] = 0;
                else
                        virtual_do[0] = 1;
        else
        if(channel == 51)
                if(value == 0)
                        virtual_do[1] = 0;
                else
                        virtual_do[1] = 1;
        else
        if(channel == 52)
                if(value == 0)
                        virtual_do[2] = 0;
                else
                        virtual_do[2] = 1;
        else
        if(channel == 53)
                if(value == 0)
                        virtual_do[3] = 0;
                else
                        virtual_do[3] = 1;
        else
        if(channel == 54)
                if(value == 0)
                        virtual_do[4] = 0;
                else
                        virtual_do[4] = 1;
        else
        if(channel == 55)
                if(value == 0)
                        virtual_do[5] = 0;
                else
                        virtual_do[5] = 1;
        else
        if(channel == 56)
                if(value == 0)
                        virtual_do[6] = 0;
                else
                        virtual_do[6] = 1;
        else
        if(channel == 57)
                if(value == 0)
                        virtual_do[7] = 0;
                else
                        virtual_do[7] = 1;
        else
        if(channel == 58)
                if(value == 0)
                        virtual_do[8] = 0;
                else
                        virtual_do[8] = 1;
        else
        if(channel == 59)
                if(value == 0)
                        virtual_do[9] = 0;
                else
                        virtual_do[9] = 1;
        else
        if(channel == 60)
                if(value == 0)
                        virtual_do[10] = 0;
                else
                        virtual_do[10] = 1;
        else
        if(channel == 61)
                if(value == 0)
                        virtual_do[11] = 0;
                else
                        virtual_do[11] = 1;
        else
        if(channel == 62)
                if(value == 0)
                        virtual_do[12] = 0;
                else
                        virtual_do[12] = 1;
        else
        if(channel == 63)
                if(value == 0)
                        virtual_do[13] = 0;
                else
                        virtual_do[13] = 1;
        else
        if(channel == 64)
                if(value == 0)
                        virtual_do[14] = 0;
                else
                        virtual_do[14] = 1;
        else
        if(channel == 65)
                if(value == 0)
                        virtual_do[15] = 0;
                else
                        virtual_do[15] = 1;
}
        

/*void write_do(unsigned short int channel, unsigned short int value)
{

        //printf("se llamo write_do con canal: %u y valor: %u\n\r",channel,value);
        if(channel == 0)
                if(value == 0)
                        PORTC.0 = 0;
                else
                        PORTC.0 = 1;
        else
        if(channel == 1)
                if(value == 0)
                        PORTC.1 = 0;
                else
                        PORTC.1 = 1;
        else
        if(channel == 2)
                if(value == 0)
                        PORTC.2 = 0;
                else
                        PORTC.2 = 1;
        else
        if(channel == 3)
                if(value == 0)
                        PORTC.3 = 0;
                else
                        PORTC.3 = 1;
        else
        if(channel == 4)
                if(value == 0)
                        PORTC.4 = 0;
                else
                        PORTC.4 = 1;
        else
        if(channel == 5)
                if(value == 0)
                        PORTC.5 = 0;
                else
                        PORTC.5 = 1;
        else
        if(channel == 6)
                if(value == 0)
                        PORTC.6 = 0;
                else
                        PORTC.6 = 1;
        else
        if(channel == 7)
                if(value == 0)
                        PORTC.7 = 0;
                else
                        PORTC.7 = 1;
        else
        if(channel == 8)
                if(value == 0)
                        PORTL = set_bit(0,PORTL,0);
                else
                        PORTL = set_bit(0,PORTL,1);
        else
        if(channel == 8)
                if(value == 0)
                        PORTL = set_bit(0,PORTL,0);
                else
                        PORTL = set_bit(0,PORTL,1);
        else
        if(channel == 9)
                if(value == 0)
                        PORTL = set_bit(1,PORTL,0);
                else
                        PORTL = set_bit(1,PORTL,1);
        else
        if(channel == 10)
                if(value == 0)
                        PORTL = set_bit(2,PORTL,0);
                else
                        PORTL = set_bit(2,PORTL,1);
        else
        if(channel == 11)
                if(value == 0)
                        PORTL = set_bit(3,PORTL,0);
                else
                        PORTL = set_bit(3,PORTL,1);
        else
        if(channel == 12)
                if(value == 0)
                        PORTL = set_bit(4,PORTL,0);
                else
                        PORTL = set_bit(4,PORTL,1);
        else
        if(channel == 13)
                if(value == 0)
                        PORTL = set_bit(5,PORTL,0);
                else
                        PORTL = set_bit(5,PORTL,1);
        else
        if(channel == 14)
                if(value == 0)
                        PORTL = set_bit(6,PORTL,0);
                else
                        PORTL = set_bit(6,PORTL,1);
        else
        if(channel == 15)
                if(value == 0)
                        PORTL = set_bit(7,PORTL,0);
                else
                        PORTL = set_bit(7,PORTL,1);
        else
        if(channel == 50)
                if(value == 0)
                        virtual_do[0] = 0;
                else
                        virtual_do[0] = 1;
        else
        if(channel == 51)
                if(value == 0)
                        virtual_do[1] = 0;
                else
                        virtual_do[1] = 1;
        else
        if(channel == 52)
                if(value == 0)
                        virtual_do[2] = 0;
                else
                        virtual_do[2] = 1;
        else
        if(channel == 53)
                if(value == 0)
                        virtual_do[3] = 0;
                else
                        virtual_do[3] = 1;
        else
        if(channel == 54)
                if(value == 0)
                        virtual_do[4] = 0;
                else
                        virtual_do[4] = 1;
        else
        if(channel == 55)
                if(value == 0)
                        virtual_do[5] = 0;
                else
                        virtual_do[5] = 1;
        else
        if(channel == 56)
                if(value == 0)
                        virtual_do[6] = 0;
                else
                        virtual_do[6] = 1;
        else
        if(channel == 57)
                if(value == 0)
                        virtual_do[7] = 0;
                else
                        virtual_do[7] = 1;
        else
        if(channel == 58)
                if(value == 0)
                        virtual_do[8] = 0;
                else
                        virtual_do[8] = 1;
        else
        if(channel == 59)
                if(value == 0)
                        virtual_do[9] = 0;
                else
                        virtual_do[9] = 1;
        else
        if(channel == 60)
                if(value == 0)
                        virtual_do[10] = 0;
                else
                        virtual_do[10] = 1;
        else
        if(channel == 61)
                if(value == 0)
                        virtual_do[11] = 0;
                else
                        virtual_do[11] = 1;
        else
        if(channel == 62)
                if(value == 0)
                        virtual_do[12] = 0;
                else
                        virtual_do[12] = 1;
        else
        if(channel == 63)
                if(value == 0)
                        virtual_do[13] = 0;
                else
                        virtual_do[13] = 1;
        else
        if(channel == 64)
                if(value == 0)
                        virtual_do[14] = 0;
                else
                        virtual_do[14] = 1;
        else
        if(channel == 65)
                if(value == 0)
                        virtual_do[15] = 0;
                else
                        virtual_do[15] = 1;
}*/

unsigned char read_do(unsigned short int channel)
{
        if(channel == 0)
                return read_bit(7,PORTL);
        else
        if(channel == 1)
                return read_bit(6,PORTL);
        else
        if(channel == 2)
                return read_bit(5,PORTL);
        else
        if(channel == 3)
                return read_bit(4,PORTL);
        else
        if(channel == 4)
                return read_bit(3,PORTL);
        else
        if(channel == 5)
                return read_bit(2,PORTL);
        else
        if(channel == 6)
                return read_bit(1,PORTL);
        else
        if(channel == 7)
                return read_bit(0,PORTL);
        else
        if(channel == 8)
                return PORTC.0;
        else
        if(channel == 9)
                return PORTC.1;
        else
        if(channel == 10)
                return PORTC.2;
        else
        if(channel == 11)
                return PORTC.3;
        else
        if(channel == 12)
                return PORTC.4;
        else
        if(channel == 13)
                return PORTC.5;
        else
        if(channel == 14)
                return PORTC.6;
        else
        if(channel == 15)
                return PORTC.7;
        else
        if(channel == 50)
                return virtual_do[0];
        else
        if(channel == 51)
                return virtual_do[1];
        else
        if(channel == 52)
                return virtual_do[2];
        else
        if(channel == 53)
                return virtual_do[3];
        else
        if(channel == 54)
                return virtual_do[4];
        else
        if(channel == 55)
                return virtual_do[5];
        else
        if(channel == 56)
                return virtual_do[6];
        else
        if(channel == 57)
                return virtual_do[7];
        else
        if(channel == 58)
                return virtual_do[8];
        else
        if(channel == 59)
                return virtual_do[9];
        else
        if(channel == 60)
                return virtual_do[10];
        else
        if(channel == 61)
                return virtual_do[11];
        else
        if(channel == 62)
                return virtual_do[12];
        else
        if(channel == 63)
                return virtual_do[13];
        else
        if(channel == 64)
                return virtual_do[14];
        else
        if(channel == 65)
                return virtual_do[15];
}

/*unsigned char read_do(unsigned short int channel)
{
        if(channel == 0)
                return PORTC.0;
        else
        if(channel == 1)
                return PORTC.1;
        else
        if(channel == 2)
                return PORTC.2;
        else
        if(channel == 3)
                return PORTC.3;
        else
        if(channel == 4)
                return PORTC.4;
        else
        if(channel == 5)
                return PORTC.5;
        else
        if(channel == 6)
                return PORTC.6;
        else
        if(channel == 7)
                return PORTC.7;
        else
        if(channel == 8)
                return read_bit(0,PORTL);
        else
        if(channel == 9)
                return read_bit(1,PORTL);
        else
        if(channel == 10)
                return read_bit(2,PORTL);
        else
        if(channel == 11)
                return read_bit(3,PORTL);
        else
        if(channel == 12)
                return read_bit(4,PORTL);
        else
        if(channel == 13)
                return read_bit(5,PORTL);
        else
        if(channel == 14)
                return read_bit(6,PORTL);
        else
        if(channel == 15)
                return read_bit(7,PORTL);
        else
        if(channel == 50)
                return virtual_do[0];
        else
        if(channel == 51)
                return virtual_do[1];
        else
        if(channel == 52)
                return virtual_do[2];
        else
        if(channel == 53)
                return virtual_do[3];
        else
        if(channel == 54)
                return virtual_do[4];
        else
        if(channel == 55)
                return virtual_do[5];
        else
        if(channel == 56)
                return virtual_do[6];
        else
        if(channel == 57)
                return virtual_do[7];
        else
        if(channel == 58)
                return virtual_do[8];
        else
        if(channel == 59)
                return virtual_do[9];
        else
        if(channel == 60)
                return virtual_do[10];
        else
        if(channel == 61)
                return virtual_do[11];
        else
        if(channel == 62)
                return virtual_do[12];
        else
        if(channel == 63)
                return virtual_do[13];
        else
        if(channel == 64)
                return virtual_do[14];
        else
        if(channel == 65)
                return virtual_do[15];
}*/

unsigned char read_di(unsigned short int channel)
{
        if(channel == 0)
                return ~PINA.0;
        else
        if(channel == 1)
                return ~PINA.1;
        else
        if(channel == 2)
                return ~PINA.2;
        else
        if(channel == 3)
                return ~PINA.3;
        else
        if(channel == 4)
                return ~PINA.4;
        else
        if(channel == 5)
                return ~PINA.5;
        else
        if(channel == 6)
                return ~PINA.6;
        else
        if(channel == 7)
                return ~PINA.7;
        else
        if(channel == 8)
                if(read_bit(0,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 9)
                if(read_bit(1,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 10)
                if(read_bit(2,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 11)
                if(read_bit(3,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 12)
                if(read_bit(4,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 13)
                if(read_bit(5,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 14)
                if(read_bit(6,PINJ) == 1)
                        return 0;
                else
                        return 1;
        else
        if(channel == 15)
                if(read_bit(7,PINJ) == 1)
                        return 0;
                else
                        return 1;
}

void write_ao(unsigned short int channel, unsigned short int value)
{
        //printf("se llamo write_ao\n\r");
        
        if(value > 0 && value <= 65535)
                if(channel == 16)
                        virtual_ao[0] = value;
                else
                if(channel == 17)
                        virtual_ao[1] = value;
                else
                if(channel == 18)
                        virtual_ao[2] = value;
                else
                if(channel == 19)
                        virtual_ao[3] = value;
                else
                if(channel == 20)
                        virtual_ao[4] = value;
                else
                if(channel == 21)
                        virtual_ao[5] = value;
                else
                if(channel == 22)
                        virtual_ao[6] = value;
                else
                if(channel == 23)
                        virtual_ao[7] = value;
                else
                if(channel == 24)
                        virtual_ao[8] = value;
                else
                if(channel == 25)
                        virtual_ao[9] = value;
                else
                if(channel == 26)
                        virtual_ao[10] = value;
                else
                if(channel == 27)
                        virtual_ao[11] = value;
                else
                if(channel == 28)
                        virtual_ao[12] = value;
                else
                if(channel == 29)
                        virtual_ao[13] = value;
                else
                if(channel == 30)
                        virtual_ao[14] = value;
                else
                if(channel == 31)
                        virtual_ao[15] = value;  
                else
                if(channel == 99)
                        a_rtc_set_time(value);
}

unsigned short int read_ao(unsigned short int channel)
{
        if(channel == 0)
                return read_adc(0);
        else
        if(channel == 1)
                return read_adc(1);
        else
        if(channel == 2)
                return read_adc(2);
        else
        if(channel == 3)
                return read_adc(3);
        else
        if(channel == 4)
                return read_adc(4);
        else
        if(channel == 5)
                return read_adc(5);
        else
        if(channel == 6)
                return read_adc(6);
        else
        if(channel == 7)
                return read_adc(7);
        else
        if(channel == 8)
                return read_adc(8);
        else
        if(channel == 9)
                return read_adc(9);
        else
        if(channel == 10)
                return read_adc(10);
        else
        if(channel == 11)
                return read_adc(11);
        else
        if(channel == 12)
                return read_adc(12);
        else
        if(channel == 13)
                return read_adc(13);
        else
        if(channel == 14)
                return read_adc(14);
        else
        if(channel == 15)
                return read_adc(15);
        else
        if(channel == 16)
                return virtual_ao[0];
        else
        if(channel == 17)
                return virtual_ao[1];
        else
        if(channel == 18)
                return virtual_ao[2];
        else
        if(channel == 19)
                return virtual_ao[3];
        else
        if(channel == 20)
                return virtual_ao[4];
        else
        if(channel == 21)
                return virtual_ao[5];
        else
        if(channel == 22)
                return virtual_ao[6];
        else
        if(channel == 23)
                return virtual_ao[7];
        else
        if(channel == 24)
                return virtual_ao[8];
        else
        if(channel == 25)
                return virtual_ao[9];
        else
        if(channel == 26)
                return virtual_ao[10];
        else
        if(channel == 27)
                return virtual_ao[11];
        else
        if(channel == 28)
                return virtual_ao[12];
        else
        if(channel == 29)
                return virtual_ao[13];
        else
        if(channel == 30)
                return virtual_ao[14];
        else
        if(channel == 31)
                return virtual_ao[15];
        else
        if(channel == 99)
                return a_rtc_get_time();
}

void calculate_humidity()
{
        raw_humidity_honey_w = read_ao(HUMIDITY_ONE);
        raw_humidity_general_e = read_ao(HUMIDITY_TWO);
         
        calculated_humidity_general_e = (((((float)raw_humidity_general_e*(float)5)/(float)1023) - (float)0.5)/(float)4) * (float)100;
        
        //calculated_humidity_general_e = (((raw_humidity_general_e - 102)*2000)/814)-500;
        
        calculated_humidity_honey_w = ((((float)raw_humidity_honey_w*(float)5)/(float)1023) - (float).958) / (float).0307;
        
        //restar 102, multiplicar por 2000, dividir entre 814 y restar  500
}

unsigned short int get_humidity()
{       
        calculate_humidity();
        //return read_ao(HUMIDITY_ONE);        
        return (calculated_humidity_general_e + calculated_humidity_honey_w)/2;
}

void calculate_temperature()
{
        raw_temperature_general_e = read_ao(TEMPERATURE);
        
        calculated_temperature_general_e = ((((float)raw_temperature_general_e*(float)5)/(float)1023 - (float)0.5)/(float)4) * (float)200 - (float)50;
        
        //calculated_temperature_general_e = raw_temperature_general_e;
}

void turn_on_fans()
{
        turn_off_state = 0;
        write_do(FAN_ONE_OFF,0);
        write_do(FAN_TWO_OFF,0);
        write_do(FAN_THREE_OFF,0);
        write_do(FAN_FOUR_OFF,0);
        write_do(FAN_FIVE_OFF,0);
        write_do(FAN_SIX_OFF,0);
        
        //printf("Turn on state: %u\n\r",turn_on_state);
        
        switch(turn_on_state)
        {
                case 0:{ 
                                write_do(FAN_ONE_ON,1);                                
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                turn_on_state++;
                                past_hs = rtc_hs;
                                tick = 0;
                                seconds = 0;
                        }; break;
                case 1:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_ONE_ON,0);
                                                write_do(FAN_ONE_STATUS_O,1);
                                                turn_on_state++;
                                        }
                        };break;
                case 2:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        //printf("Seconds: %u\n\r",seconds);
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_TWO_ON,1);                                                
                                                seconds = 0;
                                                turn_on_state++;
                                        }
                        };break;
                case 3:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_TWO_ON,0);
                                                write_do(FAN_TWO_STATUS_O,1);
                                                turn_on_state++;
                                        }
                        };break;
                case 4:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_THREE_ON,1);                                                
                                                seconds = 0;
                                                turn_on_state++;
                                        }
                        };break;
                case 5:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_THREE_ON,0);
                                                write_do(FAN_THREE_STATUS_O,1);
                                                turn_on_state++;
                                        }
                        };break;
                case 6:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_FOUR_ON,1);                                                
                                                seconds = 0;
                                                turn_on_state++;
                                        }
                        };break;
                case 7:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_FOUR_ON,0);
                                                write_do(FAN_FOUR_STATUS_O,1);
                                                turn_on_state++;
                                        }
                        };break;
                case 8:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_FIVE_ON,1);                                                
                                                seconds = 0;
                                                turn_on_state++;
                                        }
                        };break;
                case 9:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_FIVE_ON,0);
                                                write_do(FAN_FIVE_STATUS_O,1);
                                                turn_on_state++;
                                        }
                        };break;
                case 10:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_SIX_ON,1);
                                                seconds = 0;
                                                turn_on_state++;
                                        }
                        };break;
                case 11:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_SIX_ON,0);
                                                write_do(FAN_SIX_STATUS_O,1);
                                                turn_on_state++;
                                                ventilating = 1;
                                        }
                        };break;
        }

}

void turn_off_fans()
{
        turn_on_state = 0;
        write_do(FAN_ONE_ON,0);
        write_do(FAN_TWO_ON,0);
        write_do(FAN_THREE_ON,0);
        write_do(FAN_FOUR_ON,0);
        write_do(FAN_FIVE_ON,0);
        write_do(FAN_SIX_ON,0);
        //printf("Turn_off_state: %u\n\r",turn_off_state);
        
        switch(turn_off_state)
        {
                case 0:{ 
                                write_do(FAN_ONE_OFF,1);
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                turn_off_state++;
                                past_hs = rtc_hs;
                                tick = 0;
                                seconds = 0;
                        }; break;
                case 1:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_ONE_OFF,0);
                                                write_do(FAN_ONE_STATUS_O,0);
                                                turn_off_state++;
                                        }
                        };break;
                case 2:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_TWO_OFF,1);
                                                seconds = 0;
                                                turn_off_state++;
                                        }
                        };break;
                case 3:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_TWO_OFF,0);
                                                write_do(FAN_TWO_STATUS_O,0);
                                                turn_off_state++;
                                        }
                        };break;
                case 4:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_THREE_OFF,1);
                                                seconds = 0;
                                                turn_off_state++;
                                        }
                        };break;
                case 5:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_THREE_OFF,0);
                                                write_do(FAN_THREE_STATUS_O,0);
                                                turn_off_state++;
                                        }
                        };break;
                case 6:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_FOUR_OFF,1);
                                                seconds = 0;
                                                turn_off_state++;
                                        }
                        };break;
                case 7:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_FOUR_OFF,0);
                                                write_do(FAN_FOUR_STATUS_O,0);
                                                turn_off_state++;
                                        }
                        };break;
                case 8:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_FIVE_OFF,1);
                                                seconds = 0;
                                                turn_off_state++;
                                        }
                        };break;
                case 9:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_FIVE_OFF,0);
                                                write_do(FAN_FIVE_STATUS_O,0);
                                                turn_off_state++;
                                        }
                        };break;
                case 10:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= read_ao(TIME_BTW_FANS))
                                        {
                                                write_do(FAN_SIX_OFF,1);
                                                seconds = 0;
                                                turn_off_state++;
                                        }
                        };break;
                case 11:{
                                rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                                if(rtc_hs < past_hs)
                                                tick = tick + ((100 - past_hs) + rtc_hs);
                                        else
                                                tick = tick + (rtc_hs - past_hs);
                                        if(tick >= 100)
                                        {
                                                seconds++;
                                                tick = 0;
                                        }
                                        
                                        past_hs = rtc_hs;
                                        
                                        if(seconds >= FAN_PULSE)
                                        {
                                                write_do(FAN_SIX_OFF,0);
                                                write_do(FAN_SIX_STATUS_O,0);
                                                turn_off_state++;
                                                ventilating = 0;
                                        }
                        };break;
        }
}

void read_info()
{
        printf("#13\n\r");
        //calculate_humidity();
        
        printf("#%05u\n\r",raw_humidity_general_e);
        printf("#%05u\n\r",calculated_humidity_general_e);
        printf("#%05u\n\r",raw_humidity_honey_w);
        printf("#%05u\n\r",calculated_humidity_honey_w);
        calculate_temperature();
        printf("#%05u\n\r",raw_temperature_general_e);
        printf("#%05u\n\r",calculated_temperature_general_e);
        printf("#%05u\n\r",read_ao(HOUR_ONE));
        printf("#%05u\n\r",read_ao(TIME_SPAN_ONE));
        printf("#%05u\n\r",read_ao(HUMIDITY_ONE_LIMIT_L));
        printf("#%05u\n\r",read_ao(HUMIDITY_ONE_LIMIT_H));
        printf("#%05u\n\r",read_ao(TIME_BTW_FANS));
        printf("#%05u\n\r",read_ao(MIN_TIME_ON));
        printf("#%05u\n\r",read_ao(99));
        printf("#%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u\n\r",read_di(0),read_di(1),read_di(2),read_di(3),read_di(4),read_di(5),read_di(6),read_di(7),read_di(8),read_di(9),read_di(10),read_di(11),read_di(12),read_di(13),read_di(14),read_di(15));
        printf("#%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u\n\r",read_do(0),read_do(1),read_do(2),read_do(3),read_do(4),read_do(5),read_do(6),read_do(7),read_do(8),read_do(9),read_do(10),read_do(11),read_do(12),read_do(13),read_do(14),read_do(15),read_do(50),read_do(51),read_do(52),read_do(53),read_do(54),read_do(55),read_do(56));
        
        
}

muestra_hora()
{
        unsigned char h,m,s,hs;
        
        rtc_get_time(0,&h,&m,&s,&hs);
        
        printf("%u:%u:%u\n\r",h,m,s);
}

void main(void)
{
// Declare your local variables here
unsigned short int on_timer;
unsigned char last_minute;
unsigned char turning_on;
unsigned char turning_off;
unsigned char over_low_range;
unsigned char over_high_range;
unsigned char on_duty;
unsigned int serial_timer;
char letra;
char channel [5];
char value [8];
unsigned short int actual_humidity;


// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
PORTA=0xFF;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x00;
DDRC=0xFF;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x00;

// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTF=0x00;
DDRF=0x00;

// Port G initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTG=0x00;
DDRG=0x00;

// Port H initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTH=0x00;
DDRH=0x00;

// Port J initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
PORTJ=0xFF;
DDRJ=0x00;

// Port K initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTK=0x00;
DDRK=0x00;

// Port L initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTL=0x00;
DDRL=0xFF;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2A output: Disconnected
// OC2B output: Disconnected
ASSR=0x00;
TCCR2A=0x00;
TCCR2B=0x00;
TCNT2=0x00;
OCR2A=0x00;
OCR2B=0x00;

// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: Timer 3 Stopped
// Mode: Normal top=FFFFh
// Noise Canceler: Off
// Input Capture on Falling Edge
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Timer 3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

// Timer/Counter 4 initialization
// Clock source: System Clock
// Clock value: Timer 4 Stopped
// Mode: Normal top=FFFFh
// OC4A output: Discon.
// OC4B output: Discon.
// OC4C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 4 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR4A=0x00;
TCCR4B=0x00;
TCNT4H=0x00;
TCNT4L=0x00;
ICR4H=0x00;
ICR4L=0x00;
OCR4AH=0x00;
OCR4AL=0x00;
OCR4BH=0x00;
OCR4BL=0x00;
OCR4CH=0x00;
OCR4CL=0x00;

// Timer/Counter 5 initialization
// Clock source: System Clock
// Clock value: Timer 5 Stopped
// Mode: Normal top=FFFFh
// OC5A output: Discon.
// OC5B output: Discon.
// OC5C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 5 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR5A=0x00;
TCCR5B=0x00;
TCNT5H=0x00;
TCNT5L=0x00;
ICR5H=0x00;
ICR5L=0x00;
OCR5AH=0x00;
OCR5AL=0x00;
OCR5BH=0x00;
OCR5BL=0x00;
OCR5CH=0x00;
OCR5CL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
EICRA=0x00;
EICRB=0x00;
EIMSK=0x00;
// PCINT0 interrupt: Off
// PCINT1 interrupt: Off
// PCINT2 interrupt: Off
// PCINT3 interrupt: Off
// PCINT4 interrupt: Off
// PCINT5 interrupt: Off
// PCINT6 interrupt: Off
// PCINT7 interrupt: Off
// PCINT8 interrupt: Off
// PCINT9 interrupt: Off
// PCINT10 interrupt: Off
// PCINT11 interrupt: Off
// PCINT12 interrupt: Off
// PCINT13 interrupt: Off
// PCINT14 interrupt: Off
// PCINT15 interrupt: Off
// PCINT16 interrupt: Off
// PCINT17 interrupt: Off
// PCINT18 interrupt: Off
// PCINT19 interrupt: Off
// PCINT20 interrupt: Off
// PCINT21 interrupt: Off
// PCINT22 interrupt: Off
// PCINT23 interrupt: Off
PCMSK0=0x00;
PCMSK1=0x00;
PCMSK2=0x00;
PCICR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;
// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=0x00;
// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=0x00;
// Timer/Counter 3 Interrupt(s) initialization
TIMSK3=0x00;
// Timer/Counter 4 Interrupt(s) initialization
TIMSK4=0x00;
// Timer/Counter 5 Interrupt(s) initialization
TIMSK5=0x00;

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud Rate: 9600
UCSR0A=0x00;
UCSR0B=0xD8;
UCSR0C=0x06;
UBRR0H=0x00;
UBRR0L=0x67;

// USART2 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART2 Receiver: On
// USART2 Transmitter: On
// USART2 Mode: Asynchronous
// USART2 Baud Rate: 9600
UCSR2A=0x00;
UCSR2B=0xD8;
UCSR2C=0x06;
UBRR2H=0x00;
UBRR2L=0x67;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
ADCSRB=0x00;

// ADC initialization
// ADC Clock frequency: 1000.000 kHz
// ADC Voltage Reference: AREF pin
// ADC Auto Trigger Source: None
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
// ADC4: On, ADC5: On, ADC6: On, ADC7: On
DIDR0=0x00;
// Digital input buffers on ADC8: On, ADC9: On, ADC10: On, ADC11: On
// ADC12: On, ADC13: On, ADC14: On, ADC15: On
DIDR2=0x00;
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x8C;

// I2C Bus initialization
i2c_init();

// PCF8583 Real Time Clock initialization
rtc_init(0,0);

// Global enable interrupts
#asm("sei")

ventilating = 0;
        
on_timer = 0;
turning_on = 0;
turning_off = 0;

last_minute = 0;

turn_on_state = 0;
turn_off_state = 0;
over_low_range = 0;
over_high_range = 0;
on_duty = 0;

letra = '#';


while (1)
      {
      // Place your code here
        if(rx_counter0 > 0)
        {
                letra = getchar();
                
                if(letra == '$')
                {
                        //printf("Recibi Dolar\n\r");
                        serial_timer = 0;
                        
                        while(rx_counter0 == 0 && serial_timer < SERIAL_TIMEOUT)
                        {
                                serial_timer++;
                                //printf("%u\n\r",serial_timer);
                        }
                        
                        //printf("Despues de contar\n\r");
                        if(serial_timer == SERIAL_TIMEOUT && rx_counter0 == 0)
                                letra = '#';
                        else
                        {
                                letra = getchar();
                                
                                if(letra == 'w' || letra == 'W')
                                {
                                        //printf("Recibi W\n\r");
                                        serial_timer = 0;
                                        
                                        while(rx_counter0 < 2 && serial_timer < SERIAL_TIMEOUT)
                                                serial_timer++;
                                        
                                        if(serial_timer == SERIAL_TIMEOUT && rx_counter0 < 2)
                                                letra = '#';
                                        else
                                        {
                                                letra = getchar();
                                                
                                                if(letra == 'a' || letra == 'A')
                                                {
                                                        //printf("Recibi A\n\r");
                                                        letra = getchar();
                                                        
                                                        if(letra == 'o' || letra == 'O')
                                                        {
                                                                //printf("Recibi O\n\r");
                                                                serial_timer = 0;
                                                                
                                                                while(rx_counter0 < 7 && serial_timer < SERIAL_TIMEOUT)
                                                                        serial_timer++;
                                                                
                                                                if(serial_timer == SERIAL_TIMEOUT && rx_counter0 < 7)
                                                                {
                                                                        letra = '#';
                                                                        //printf("Se dio un timeout de 7\n\r");
                                                                }
                                                                else
                                                                {
                                                                        channel[0] = getchar();
                                                                        channel[1] = getchar();
                                                                        channel[2] = '\0';
                                                                        
                                                                        value[0] = getchar();
                                                                        value[1] = getchar();
                                                                        value[2] = getchar();
                                                                        value[3] = getchar();
                                                                        value[4] = getchar();
                                                                        value[5] = '\0';
                                                                        
                                                                        write_ao(atoi(channel), atoi(value));
                                                                }
                                                        }
                                                        else
                                                                letra = '#';
                                                }
                                                else
                                                if(letra == 'd' || letra == 'D')
                                                {
                                                        //printf("Recibi D\n\r");
                                                        letra = getchar();
                                                        
                                                        if(letra == 'o' || letra == 'O')
                                                        {
                                                                //printf("Recibi O\n\r");
                                                                serial_timer = 0;
                                                                
                                                                while(rx_counter0 < 3 && serial_timer < SERIAL_TIMEOUT)
                                                                        serial_timer++;
                                                                
                                                                if(serial_timer == SERIAL_TIMEOUT && rx_counter0 < 3)
                                                                        letra = '#';
                                                                else
                                                                {
                                                                        channel[0] = getchar();
                                                                        channel[1] = getchar();
                                                                        channel[2] = '\0';
                                                                        
                                                                        value[0] = getchar();
                                                                        value[1] = '\0';
                                                                        
                                                                        write_do(atoi(channel), atoi(value));
                                                                }
                                                        }
                                                        else
                                                                letra = '#';
                                                }
                                                else
                                                        letra = '#';
                                        }
                                }
                                else
                                if(letra == 'r' || letra == 'R')
                                {
                                        //printf("Recibi R\n\r");
                                        read_info();
                                        letra = '#';
                                }
                                if(letra == 'p' || letra == 'P')
                                {
                                        //printf("Recibi P\n\r");
                                        muestra_hora();
                                        letra = '#';
                                }
                                else
                                        letra = '#';                                
                        }
                }
        }
        
        
        
        
        
        
        /************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************
        *************************************************************************************************************************************/
        
        actual_humidity = get_humidity();
        
        if(read_di(SWITCH_AUTO) == 0)
        {
                //printf("switch puesto en manual\n\r");
                
                if(read_di(SWITCH_ON) == 1)
                {
                        //printf("Switch en on activado\n\r");
                        turn_on_fans();
                }
                else
                {
                        //printf("Switch en off activado\n\r");
                        turn_off_fans();
                }
                on_duty = 0;
                on_timer = 0;
        }
        else
        {    
                //printf("Switch en auto\n\r");
                if(read_do(CANCEL_AUT) == 1)
                {
                        turn_off_fans();
                        on_duty = 0;
                        on_timer = 0;
                }
                else
                {
                        
                        
                        rtc_get_time(0,&rtc_h, &rtc_m, &rtc_s, &rtc_hs);
                
                        if(actual_humidity >= read_ao(HUMIDITY_ONE_LIMIT_L) && actual_humidity <= read_ao(HUMIDITY_ONE_LIMIT_H))
                        {
                                //printf("En rango de humedad\n\r");
                                if(read_ao(TIME_SPAN_ONE) > 0)
                                {
                                        
                                        //printf("%u\n\r",(unsigned short int)rtc_h*60);
                                        //if((((rtc_h*60)+rtc_m) >= read_ao(HOUR_ONE)) && ((rtc_h*60)+rtc_m) <= (read_ao(HOUR_ONE) + read_ao(TIME_SPAN_ONE)))
                                        if(((unsigned short int)rtc_h*60) + (unsigned short int)rtc_m >= read_ao(HOUR_ONE))
                                        { 
                                                 //printf("Mas alto que tiempo inicial\n\r");
                                                if((unsigned short int)rtc_h*60 + (unsigned short int)rtc_m <= read_ao(HOUR_ONE) + read_ao(TIME_SPAN_ONE))
                                                {
                                                       //printf("En rango de tiempo\n\r");
                                                       turn_on_fans();
                                                       if(on_duty == 0 && on_timer != read_ao(MIN_TIME_ON))
                                                       {
                                                               on_duty = 1;
                                                               on_timer = 0;
                                                               last_minute = rtc_m;
                                                       }
                                                       else
                                                       {
                                                               if(last_minute != rtc_m && on_timer != read_ao(MIN_TIME_ON))
                                                               {
                                                                       on_timer++;
                                                                       last_minute = rtc_m;
                                                               }
                                                               if(on_timer == read_ao(MIN_TIME_ON))
                                                               {

                                                                  on_duty = 0;
                                                                  
                                                               }
                                                       }
                                                }
                                                else
                                                {
                                                         turn_off_fans();
                                                         on_duty = 0;
                                                         on_timer = 0;
                                                }
                                        }
                                        else
                                        {
                                                turn_off_fans();
                                                on_duty = 0;
                                                                  on_timer = 0;
                                        }
                                }
                                else
                                {
                                        turn_off_fans();
                                        on_duty = 0;
                                                                  on_timer = 0;
                                }
                        }
                        else
                        {           
                                  //printf("Valor de on duty:%u\n\r",on_duty);
                                if(on_duty)
                                {
                                          
                                        turn_on_fans();
                                        
                                        //printf("lkasklaska\n\r");
                                        
                                        if(on_timer != read_ao(MIN_TIME_ON))
                                        {
                                                if(last_minute != rtc_m)
                                                {
                                                        on_timer++;
                                                        last_minute = rtc_m;
                                                }
                                        }
                                        else
                                        {
                                                  //printf("Se acabo el ciclo\n\r");
                                                on_duty = 0;
                                                                  on_timer = 0;
                                        }
                                }
                                else
                                {
                                          turn_off_fans();
                                          on_timer = 0;
                                          
                                }
                        }
                }
        }
        
        
       
      };
}

