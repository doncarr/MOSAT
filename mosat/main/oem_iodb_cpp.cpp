/*
 * oem_iodb.c
 *
 * danm@sixnetio.com - (03/20/2002)
 *
 * This code is written to give an example or two on
 * how to access the SIXNET I/O database from external
 * OEM applications.
 *
 * NOTE: Discretes are bitpacked.  8 bits per byte.
 *       Floats are treated as floats (4 bytes), not doubles.
 *       Longs are treated as longs (4 bytes).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "iodb_cpp.h"

/***
extern "C" 
{
#include <oemdefs.h>
#include <iodb.h>
}
***/


int main()
{
    int i;
    UINT16 addr;		/* Starting Register */
    UINT16 regs;		/* Number of Register */
    UINT16 datatype;	/* Type of I/O */
    UINT16 typesize;
    UINT16 typenum;
    UINT16 ecode;
    UINT16 a_value;
    UINT16 analog_values[8];
    BYTE discrete_values;
    char tagname[32];


    /*
     * Write 1 analog value to analog output register 0.
     * Then read the value back in...
     */
    addr = 0;
    regs = 1;
    a_value = 37;
    printf("\nWriting 1 analog, Reading 1 back...\n");
    printf("a_value: %d\n", a_value);
    ecode = IODBWrite(ANALOG_OUT, addr, regs, (void *)&a_value, NULL);
    a_value = 0;
    ecode = IODBRead(ANALOG_OUT, addr, regs, (void *)&a_value, NULL);
    printf("a_value (returned): %d\n", a_value);


    /*
     * Write 3 analog values to 3 consecutive registers,
     * starting at analog output register 1.
     * Then read the values back from register 0.
     */
    addr = 1;
    regs = 3;
    analog_values[0] = 11;
    analog_values[1] = 22;
    analog_values[2] = 33;
    printf("\nWriting 3 analogs, Reading 4 back...\n");
    for(i = 0; i < regs; i++)
        printf("analog_values[%d]: %d\n", i, analog_values[i]);
    ecode = IODBWrite(ANALOG_OUT, addr, regs, (void *)&analog_values, NULL);
    printf("ecode = %d\n", (int) ecode);
    memset((void *)&analog_values, '0', sizeof(INT16) * 8);
    addr = 0;
    regs = 4;
    ecode = IODBRead(ANALOG_OUT, addr, regs, (void *)&analog_values, NULL);
    printf("ecode = %d\n", (int) ecode);
    for(i = 0; i < regs; i++)
        printf("analog_values[%d]: %d\n", i, analog_values[i]);


    /*
     * Write 1 discrete value to discrete output register 0.
     * Then read the value back in...
     */
    addr = 0;
    regs = 1;
    discrete_values = 1;  /* Turning discrete ON */
    printf("\nWriting 1 discrete, Reading 1 back...\n");
    printf("discrete_values: %d\n", discrete_values);
    ecode = IODBWrite(DISCRETE_OUT, addr, regs, (void *)&discrete_values, NULL);
    printf("ecode = %d\n", (int) ecode);
    discrete_values = 0;
    ecode = IODBRead(DISCRETE_OUT, addr, regs, (void *)&discrete_values, NULL);
    printf("ecode = %d\n", (int) ecode);
    printf("discrete_values (returned): %d\n", discrete_values);


    /*
     * Write 4 discrete value to discrete output register 4.
     * Then read 8 discretes back, starting at register 0...
     *    D0 = ON  (Previous Write)
     *    D4 = ON
     *    D6 = ON
     *    If nothing else is ON, should return 81.
     */
    addr = 4;
    regs = 4;
    discrete_values = 5;  /* D0 & D2 are on, D1 & D3 are off */
    printf("\nWriting 4 discretes, Reading 8 back...\n");
    printf("discrete_values: %d\n", discrete_values);
    ecode = IODBWrite(DISCRETE_OUT, addr, regs, (void *)&discrete_values, NULL);
    printf("ecode = %d\n", (int) ecode);
    discrete_values = 0;
    addr = 0;
    regs = 8; 
    ecode = IODBRead(DISCRETE_OUT, addr, regs, (void *)&discrete_values, NULL);
    printf("ecode = %d\n", (int) ecode);
    printf("discrete_values (returned): %d\n", discrete_values);


    /*
     * Set a tag to correspond to an discrete output register
     */
    strcpy(tagname, "do_tag");
    addr = 2;  /* Start at register 2 */
    regs = 1;  /* Only one register */
    printf("\nAssigning tagname (%s) to discrete output register 2...\n",
            tagname);
    ecode = IODBSetTag(tagname, DISCRETE_OUT, addr, regs);
    printf("ecode = %d\n", (int) ecode);

    /*
     * Write one discrete output value to tagname.
     * Then read back the same discrete output value, via tagname.
     */
    discrete_values = 1;  /* Turning discrete ON */
    printf("\nWriting/Reading 1 discrete output values to/from %s...\n",
            tagname);
    printf("discrete_values: %d\n", discrete_values);
    ecode = IODBWriteTag(tagname, (void *)&discrete_values);
    printf("ecode = %d\n", (int) ecode);
    discrete_values = 0;
    ecode = IODBReadTag(tagname, (void *)&discrete_values,
            sizeof(discrete_values), NULL, NULL);
    printf("ecode = %d\n", (int) ecode);
    printf("discrete_values associated with %s (returned): %d\n",
            tagname, discrete_values);


    /*
     * Set a tag to correspond to a block of analog output registers
     */
    strcpy(tagname, "ao_tag");
    addr = 4;  /* Start at register 4 */
    regs = 4;  /* 4 consecutive registers */
    printf("\nAssigning tagname (%s) to analog output registers: 4, 5, 6, 7\n",
            tagname);
    ecode = IODBSetTag(tagname, ANALOG_OUT, addr, regs);
    printf("ecode = %d\n", (int) ecode);


    /*
     * Write 4 analogs output values to tagname
     * Then read back the same 4 analog output values, via tagname.
     */
    analog_values[0] = 9;
    analog_values[1] = 8;
    analog_values[2] = 7;
    analog_values[3] = 6;
    printf("\nWriting/Reading 4 analog output values to/from %s...\n",
            tagname);
    printf("Datatype & the numbers of registers will also be returned.\n");
    for(i = 0; i < 4; i++)
        printf("analog_values[%d]: %d\n", i, analog_values[i]);
    ecode = IODBWriteTag(tagname, (void *)&analog_values);
    printf("ecode = %d\n", (int) ecode);
    memset((void *)&analog_values, '0', sizeof(INT16) * 8);
    ecode = IODBReadTag(tagname, (void *)&analog_values,
            sizeof(analog_values), &datatype, &regs);
    printf("ecode = %d\n", (int) ecode);
    printf("%d registers, of type (%d), are associated with %s...\n",
            regs, datatype, tagname);
    for(i = 0; i < regs; i++)
        printf("analog_values[%d]: %d\n", i, analog_values[i]);


    /*
     * Get information related to tagname
     * Any unwanted values can be NULL'ed out
     */
    printf("\nGetting information associated with %s...\n", tagname);
    ecode = IODBGetTag(tagname, NULL, &typenum, &datatype, &typesize,
            &regs, &addr);
    printf("ecode = %d\n", (int) ecode);
    printf("Retrieved the following information:\n");
    printf("\ttypenum: %d\n", typenum);
    printf("\tdatatype: %d\n", datatype);
    printf("\ttypesize: %d\n", typesize);
    printf("\tregs: %d\n", regs);
    printf("\taddr: %d\n\n", addr);



    exit (0);
}




