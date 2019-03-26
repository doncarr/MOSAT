
export CPATH=../include

CPP=g++ -Wall -Werror

INCLUDES = alg.h controlalg.h dc_alg.h event.h jg_alg.h section.h sim.h xx_alg.h startup_alg.h

RTCOMMON=../librtcommon/librtcommon.a

all: sim sched rtu

%.o:	%.cpp
	$(CPP) -c -o $@ $<

sim: sim.o section.o dc_alg.o controlalg.o jg_alg.o startup_alg.o jg_sec_colection.cpp $(INCLUDES)
	$(CPP) -o sim -lncurses sim.o section.o dc_alg.o controlalg.o jg_alg.o jg_sec_colection.cpp startup_alg.o $(RTCOMMON) -lncurses

sched:	sched.cpp section.o $(INCLUDES)
	$(CPP) -o sched sched.cpp section.o $(RTCOMMON)

rtu:	rtu.cpp $(INCLUDES)
	$(CPP) -o rtu rtu.cpp

tdisplay: tdisplay.o sim_train.o $(INCLUDES)
	$(CPP) -o tdisplay -lncurses tdisplay.o sim_train.o

clean: 
	rm sim sched rtu *.o

