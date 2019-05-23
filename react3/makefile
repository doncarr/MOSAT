
OBJS = 

SRCS =

.PHONY: clean all

export CPATH=../include

RTCOMMON=../librtcommon/librtcommon.a

INCLUDES =

LIBS = -ldl $(RTCOMMON)

LDFLAGS =

CPP = g++ $(CFLAGS)

MAKE = make

%.o:    %.cpp $(INCLUDES)
	$(CPP) -c  -o $@ $<

all: react3


react3: $(INCLUDES) $(OBJS) reactmain.cpp
	$(CPP) $(LDFLAGS) -rdynamic -o $@ $(OBJS) reactmain.cpp -lpthread $(LIBS)

clean:
	rm -fv *.o *.so react3 
