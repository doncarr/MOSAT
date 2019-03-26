# Makefile for react.
OBJ1 = reactmain.o db.o di.o do.o ao.o data.o file_logger.o web_logger.o discrete_logger.o scan.o dcalc.o discrete_calc.o shutdown.o alarmwin.o pointstate.o react_sms.o
OBJ2 = secuencia.o iodriver.o pid.o secuencia_step.o logfile.o control.o web.o displaydata.o script_support.o gen_case.o gen_script.o conio.o
OBJ3 =	int.o pci.o ldiodriver.o timeaccum.o error.o eval.o expr.o exprlist.o exprtree.o point.o  calc.o lddbpoint.o analog.o discrete.o ac.o pump.o level.o timer_point.o analog_value.o discrete_value.o
OBJS =  $(OBJ1) $(OBJ2) $(OBJ3)

SRC1 =	reactmain.cpp db.cpp di.cpp do.cpp ao.cpp data.cpp scan.cpp dcalc.cpp discrete_calc.cpp shutdown.cpp alarmwin.cpp pointstate.cpp react_sms.cpp
SRC2 = secuencia.cpp iodriver.cpp pid.cpp secuencia_step.cpp logfile.cpp control.cpp web.cpp displaydata.cpp
SRC3 = script_support.cpp gen_case.cpp gen_script.cpp conio.cpp int.cpp
SRC4 =	pci.cpp ldiodriver.cpp timeaccum.cpp error.cpp eval.cpp expr.cpp exprlist.cpp exprtree.cpp point.cpp calc.cpp lddbpoint.cpp analog.cpp discrete.cpp ac.cpp pump.cpp level.cpp timer_point.cpp analog_value.cpp discrete_value.cpp
SRCS =  $(SRC1) $(SRC2) $(SRC3) $(SRC4)

export CPATH=../include

RTCOMMON=../librtcommon/librtcommon.a

INCLUDES = db.h db_point.h iodriver.h logfile.h\
error.h exp.h reactpoint.h secuencia.h displaydata.h timeaccum.h gen_script.h

#LIBS = -l pci_dask -ldl 
LIBS = -ldl $(RTCOMMON) 

GENFILES =  gen_case.cpp gen_script.cpp gen_script.h

DRIVERDIRS = drivers/pci9111 drivers/pci7248 drivers/sim drivers/modbusdrv drivers/shmdrv drivers/sim_pump drivers/sim_pump_low 

DRIVERDIRS2 = drivers/sim_tina drivers/dallas_1wire drivers/sim_ac drivers/process drivers/simple_ascii drivers/simple_ascii_new 

CDEBUG =

CFLAGS = -Wall -Werror 
#CFLAGS = -Wall -Wextra -Wold-style-cast
#CFLAGS=-Wall -Wstrict-prototypes -O2

LDFLAGS =

CPP = g++  -fPIC $(CFLAGS)


MAKE = make

all: react integrate utest rconsole reactcgi rdrivers drivers2 rtshmmain ai.so

%.o:	%.cpp $(INCLUDES)
	$(CPP) -c  -o $@ $<

rdrivers:
	for i in $(DRIVERDIRS); do cd "$$i"; $(MAKE); cd ../..; done

drivers2: 
	for i in $(DRIVERDIRS2); do cd "$$i"; $(MAKE); cd ../..; done

react:	$(INCLUDES) $(OBJS) dbpoint_util.so
	echo \"`svn info | grep -i revision | awk '{print $2}'`\" > revision_number
	$(CPP) $(LDFLAGS) -rdynamic -o $@ $(OBJS) $(LIBS) -lrt

rtshmmain: rtshmmain.o ldiodriver.o reactshmio.o logfile.o reactshmio.h $(INCLUDES)
	$(CPP) -rdynamic -o rtshmmain rtshmmain.o ldiodriver.o reactshmio.o logfile.o -ldl $(RTCOMMON) -lrt

$(OBJS):	$(INCLUDES)

secuencia:	secuencia.cpp
	$(CPP) -o secuencia  secuencia.cpp $(RTCOMMON) 


test:	pid.cpp 
	$(CPP) -o test pid.cpp

integrate:	integrate.cpp  
	$(CPP) -o integrate integrate.cpp $(RTCOMMON)


p9111ai: p9111ai.o conio.o
	$(CPP) -o p9111ai -L/usr/lib -lpci_dask p9111ai.o conio.o

p9111ai.o: p9111ai.c
	$(CPP) -c $(CFLAGS) -I../../../include p9111ai.c

dbpoint_util.so: analog.cpp discrete.cpp
	$(CPP) -o dbpoint_util.so -shared analog.cpp discrete.cpp 

gen:	generate_script.cpp 
	$(CPP) -o gen  generate_script.cpp $(RTCOMMON)

$(GENFILES):	gen db_point.h
	./gen

TAGS:   $(SRCS)
	etags $(SRCS)


genc:	gen_script.cpp gen_script.h
	$(CPP) -c gen_script.cpp 

avg:	average.cpp
	$(CPP) -o avg average.cpp $(RTCOMMON) 

gencase:	gen_script.cpp gen_script.h
	$(CPP) -c gen_case.cpp 

utest:	utest.cpp
	$(CPP) -o utest utest.cpp $(RTCOMMON) -lrt
	
ct:	conio.cpp conio.h coniotest.cpp
	$(CPP) -o ct coniotest.cpp conio.cpp

simtest:	simtest.cpp burnersim.cpp sim.h
	$(CPP) -o simtest simtest.cpp burnersim.cpp

genhtml:	genhtml.cpp
	$(CPP) -o genhtml genhtml.cpp $(RTCOMMON) 

rconsole: reactconsole.cpp
	$(CPP) -o rconsole reactconsole.cpp

rdisplay: rtdisplay.cpp displaydata.cpp
	$(CPP) -o rdisplay -l ncurses rtdisplay.cpp displaydata.cpp

cgiTest: cgiTest.c
	$(CPP) -o  cgiTest cgiTest.c

reactcgi: reactcgi.cpp
	$(CPP) -o reactcgi reactcgi.cpp 

usbtest: usbtest.cpp
	$(CPP)  -W usbtest.cpp

split_fields:	split_fields.cpp
	$(CPP) -o split_fields split_fields.cpp $(RTCOMMON)
	cp split_fields /home/carr/bin

gen_images:	gen_images.cpp
	$(CPP) -o gen_images -lrtcomon gen_images.cpp
	cp gen_images /home/carr/bin

exprtest:       error.h exp.h error.cpp eval.cpp expr.cpp exprlist.cpp exprtree.cpp point.cpp treetst.cpp
	$(CPP) -o exprtest error.cpp eval.cpp expr.cpp exprlist.cpp exprtree.cpp point.cpp treetst.cpp

ai.so:	ai.cpp
	$(CPP) -shared -o ai.so ai.cpp

script_trigger.so:	script_trigger.cpp
	$(CPP) -shared -o script_trigger.so script_trigger.cpp

gen_php:	gen_php.cpp
	$(CPP) -g  -o gen_php gen_php.cpp $(RTCOMMON)

clean:
	rm -fv *.o *.so react rconsole rdisplay reactcgi gen utest integrate
	rm -fv rtshmmain gen_case.cpp gen_script.cpp gen_script.h gen_php
	for i in $(DRIVERDIRS) drivers/sixnet_rtu ; do cd "$$i"; $(MAKE) clean; cd ../..; done
	for i in $(DRIVERDIRS2); do cd "$$i"; $(MAKE) clean; cd ../..; done


install:
	cp react /usr/local/bin
	cp rtshmmain /usr/local/bin
	cp rconsole /usr/local/bin
	cp rdisplay /usr/local/bin
	cp ai.so /usr/local/lib

ppcall:
	make -f makefile.ppc

ppcclean:
	make -f makefile.ppc clean

armall:
	make -f makefile.arm

armclean:
	make -f makefile.arm clean


