
export CPATH=../../include

CC=g++ -g -Wall

CPP=g++ -g -Wall

RTCOMMON=../../librtcommon/librtcommon.a
RTMODBUS=../../modbus/librtmodbus.a

%.o:    %.cpp $(INCLUDES)
	$(CPP) -c -o $@ -Wall $<

%.o:    %.c $(INCLUDES)
	$(CC) -c -o $@ -Wall $<

all: gen_profiles calc_savings

calc_savings: calc_savings.cpp
	g++ -Wall -o calc_savings calc_savings.cpp

gen_profiles:	gen_profiles.o restriction_reader.o 
	g++ -g -o gen_profiles gen_profiles.o restriction_reader.o $(RTMODBUS) $(RTCOMMON)

clean: 
	rm -v *.o gen_profiles calc_savings gnuplotoptions.txt  plot.txt  profile_out.txt
