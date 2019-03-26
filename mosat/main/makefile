INCLUDES = hp2mod.h  hprtu.h  modio.h  modsim.h  mtype.h  trace.h

export CPATH=../../include

RTCOMMON=../../librtcommon/librtcommon.a

CC = g++ -Wall

ALL= remote_record ntt runit ppc_record rtsync auto_report frames cambiahora

all:	$(ALL)

%.o:	%.cpp $(INCLUDES)
	$(CC) -c -o $@ -Wall $<

ppc_record: ppc_record.o train_detect.o evlog.o train_detect.h ppc_logger.o ppc_logger.h event_detect.o
	g++ -o ppc_record -lpthread ppc_record.o evlog.o train_detect.o \
		ppc_logger.o event_detect.o $(RTCOMMON) -lrt -lpthread

remote_record: remote_record.o train_detect.o evlog.o train_detect.h o_evlog.o
	g++ -o remote_record -lpthread remote_record.o \
	   o_evlog.o evlog.o train_detect.o  $(RTCOMMON) -lrt  -lpthread

ntt:	timetable.cpp rf.cpp n_ractual.cpp
	g++ -o ntt -Wall timetable.cpp rf.cpp n_ractual.cpp $(RTCOMMON) -lrt -lpthread

runit:	runit.cpp
	g++ -o runit runit.cpp -lrt -lpthread

clean:	
	rm -fv *.o *.so $(ALL) 
	make -f makefile.ppc clean


ppc_all:
	make -f makefile.ppc

cgi:
	make -f makefile.cgi

rtsync:	rtsync.cpp
	g++ -Wall -o rtsync rtsync.cpp $(RTCOMMON)

auto_report:	auto_report.cpp
	g++ -Wall -o auto_report auto_report.cpp

frames:	frames.cpp
	g++ -Wall -o frames frames.cpp $(RTCOMMON)

mbiahora:	cambiahora.cpp
	g++ -Wall -o cambiahora cambiahora.cpp $(RTCOMMON)


