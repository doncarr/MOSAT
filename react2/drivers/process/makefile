
export CPATH=../../../include:../../

all: libprocess_drv.so 

process_drv.o:	process_drv.cpp process_drv.h
	g++ -std=c++0x -Wall -fPIC -c process_drv.cpp 

process_mq.o:	process_mq.cpp process_drv.h
	g++ -std=c++0x -Wall -fPIC -c process_mq.cpp 

libprocess_drv.so: process_drv.o process_mq.o
	g++ -std=c++0x -Wall -o libprocess_drv.so -shared process_drv.o process_mq.o -lrt ../../../modbus/librtmodbus.a 

process_main: process_main.cpp
	g++ -std=c++0x -Wall -o process_main process_main.cpp ../../ldiodriver.o ../../logfile.o \
              -lrt -ldl ../../../librtcommon/librtcommon.a ../../../modbus/librtmodbus.a

mq_check: mq_check.cpp
	g++ -std=c++0x -Wall -o mq_check mq_check.cpp \
              -lrt -ldl ../../../librtcommon/librtcommon.a

clean:
	rm *.o *.so process_main


