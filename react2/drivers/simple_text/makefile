
export CPATH=../../../include:../../

RTCOMMON=../../../librtcommon/librtcommon.a
RTMODBUS=../../../modbus/librtmodbus.a

all: sim_simple_text libsimple_text_drv.so

simple_text.o:	simple_text.cpp simple_text.h
	g++ -std=c++0x -Wall -fPIC -c simple_text.cpp 

simple_text_read.o:	simple_text_read.cpp simple_text.h
	g++ -std=c++0x -Wall -fPIC -c simple_text_read.cpp 

libsimple_text_drv.so: simple_text.o simple_text_read.o
	g++ -fPIC -std=c++0x -Wall -o libsimple_text_drv.so -shared simple_text.o simple_text_read.o -lrt $(RTMODBUS) 

sim_simple_text: sim_simple_text.cpp  
	g++ -std=c++0x -Wall -o sim_simple_text sim_simple_text.cpp -lrt $(RTMODBUS) 

temps: temps.o
	g++ -fPIC -std=c++0x -g -Wall -lrt -o temps temps.o $(RTMODBUS) $(RTCOMMON) 

convert: convert.cpp
	g++ -std=c++0x -o convert convert.cpp $(RTCOMMON) 

clean:
	rm *.o *.so sim_simple_text


