

export CPATH=../include

RTCOMMON=../librtcommon/librtcommon.a
RTMODBUS=./librtmodbus.a

INCLUDES = hp2mod.h  hprtu.h  modio.h  modsim.h  mtype.h  trace.h evlog.h

CPP=g++ -Wall -Werror -std=c++0x -fPIC

%.o:	%.cpp $(INCLUDES)
	$(CPP) -c -o $@ $<

all:	 librtmodbus.so librtmodbus.a modsim myconnect tst_serial_master tst_serial_slave tst_radio_master tst_radio_slave 

modsim:	myservice.o modsim.o trace.h 
	$(CPP) -o modsim myservice.o $(RTMODBUS) $(RTCOMMON)  -lpthread -lrt

myconnect:	myconnect.o
	$(CPP) -o myconnect myconnect.o  $(RTMODBUS) $(RTCOMMON) -lrt

librtmodbus.so: trace.o modsim.o trace.h swap.o modtcpip_client.o\
                modtcpip.o hexascii.o rtmodbus.o modio.o \
                evlog.o rt_serial.o rt_mod_serial_client.o rt_modbus_utility.o $(INCLUDES)
	$(CPP) -o librtmodbus.so -shared modsim.o trace.o swap.o \
		 modtcpip.o hexascii.o rtmodbus.o modio.o modtcpip_client.o \
		evlog.o rt_serial.o rt_mod_serial_client.o rt_modbus_utility.o

librtmodbus.a: trace.o modsim.o trace.h swap.o modtcpip_client.o\
                modtcpip.o hexascii.o rtmodbus.o modio.o \
                evlog.o rt_serial.o rt_mod_serial_client.o rt_modbus_utility.o $(INCLUDES)
	ar r librtmodbus.a modsim.o trace.o swap.o \
		 modtcpip.o hexascii.o rtmodbus.o modio.o modtcpip_client.o \
		evlog.o rt_serial.o rt_mod_serial_client.o rt_modbus_utility.o

tst_serial_slave: tst_serial_slave.cpp $(INCLUDES)
	$(CPP) -o tst_serial_slave tst_serial_slave.cpp -lrt ./librtmodbus.a

tst_radio_slave: tst_radio_slave.cpp $(INCLUDES)
	$(CPP) -o tst_radio_slave tst_radio_slave.cpp ./librtmodbus.a -lrt


tst_serial_master:  tst_serial_master.cpp $(INCLUDES)
	$(CPP) -o tst_serial_master tst_serial_master.cpp ./librtmodbus.a -lrt

find_serial_ports:  find_serial_ports.cpp $(INCLUDES)
	$(CPP) -o find_serial_ports find_serial_ports.cpp ./librtmodbus.a -lrt

tst_radio_master:  tst_radio_master.cpp $(INCLUDES)
	$(CPP) -o tst_radio_master tst_radio_master.cpp  ./librtmodbus.a -lrt

install:
	cp librtmodbus.so /usr/local/lib
	cp rtmodbus.h /usr/local/include
	cp modsim /usr/local/bin


clean:	
	rm -fv *.o modsim myconnect librtmodbus.so librtmodbus.a tst_serial_slave tst_serial_master tst_radio_master tst_radio_slave

ppcall:
	make -f makefile.ppc

ppcclean:
	make -f makefile.ppc clean

armall:
	make -f makefile.arm

armclean:
	make -f makefile.arm clean


