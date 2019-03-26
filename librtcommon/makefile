
export CPATH=../include

OBJS=  arg.o common.o safestr.o utimer.o memfail.o  numbered_file.o ap_config.o separator.o rt_system_log.o log_functions.o split_array.o random_tools.o

CPP=g++ -fPIC -Wall -Werror

%.o:    %.cpp $(INCLUDES)
	$(CPP) -c -o $@ -Wall $<

all: librtcommon.so librtcommon.a 

librtcommon.so: $(OBJS) 
	$(CPP) -shared -o librtcommon.so $(OBJS) 

librtcommon.a: $(OBJS) 
	  ar r librtcommon.a $(OBJS) 

install:
	cp librtcommon.so /usr/local/lib

uninstall:
	cp /usr/local/lib/librtcommon.so

installppc:
	make -f makefile.ppc install

installarm:
	make -f makefile.arm install

clean:
	rm -fv *.o librtcommon.so librtcommon.a
	make -f makefile.ppc clean 
	make -f makefile.arm clean

ppcall:
	make -f makefile.ppc
	make librtcommon.a

armall:
	make -f makefile.arm
	make librtcommon.a

ppcclean:
	make -f makefile.ppc clean

armclean:
	make -f makefile.arm clean

