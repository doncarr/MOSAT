
export CPATH=../include:/usr/include/mysql

CC=g++ -g -Wall



RTCOMMON=../librtcommon/librtcommon.so

%.o:    %.cpp $(INCLUDES)
	$(CC) -c -g -o $@ $<

all: librtsms.so librtsms.a testsms testsms_so

testsms: testsms.cpp sms.cpp ../include/sms.h sms_implement.h
	$(CC) -o testsms testsms.cpp sms.cpp -lmysqlclient -lnsl -lm -lz $(RTCOMMON)

testsms_so: testsms_so.cpp ../include/sms.h
	$(CC) -o testsms_so testsms_so.cpp -lmysqlclient -lnsl -lm -lz -ldl $(RTCOMMON)
	
librtsms.so: sms.o ../include/sms.h sms_implement.h
	g++  -shared -o librtsms.so sms.o 

librtsms.a: sms.o ../include/sms.h sms_implement.h
	  ar r librtsms.a sms.o

clean:
	rm -fv sms *.o testsms testsms_so


