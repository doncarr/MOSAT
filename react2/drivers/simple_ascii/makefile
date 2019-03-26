
export CPATH=../../../include:../../

all: sim_simple_ascii libsimple_ascii_drv.so

simple_ascii.o:	simple_ascii.cpp simple_ascii.h
	g++ -std=c++0x -Wall -fPIC -c simple_ascii.cpp 

simple_ascii_read.o:	simple_ascii_read.cpp simple_ascii.h
	g++ -std=c++0x -Wall -fPIC -c simple_ascii_read.cpp 

libsimple_ascii_drv.so: simple_ascii.o simple_ascii_read.o
	g++ -std=c++0x -fPIC -Wall -o libsimple_ascii_drv.so -shared simple_ascii.o simple_ascii_read.o -lrt ../../../modbus/librtmodbus.a 

sim_simple_ascii: sim_simple_ascii.cpp  
	g++ -std=c++0x -Wall -o sim_simple_ascii sim_simple_ascii.cpp -lrt ../../../modbus/librtmodbus.a

temps: temps.o
	g++ -std=c++0x -g -Wall -lrt -o temps temps.o ../../../modbus/librtmodbus.a ../../../librtcommon/librtcommon.a

convert: convert.cpp
	g++ -std=c++0x -o convert convert.cpp ../../../librtcommon/librtcommon.a 

clean:
	rm *.o *.so sim_simple_ascii


