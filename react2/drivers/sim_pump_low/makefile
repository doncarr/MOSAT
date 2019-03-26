
export CPATH=../../../include:../../

libsimpumpdrv.so: simpumpdrv.cpp
	g++ -fPIC -std=c++0x -Wall -o libsimpumpdrv.so -shared simpumpdrv.cpp  ../../../librtcommon/librtcommon.a

clean:
	rm -fv libsimpumpdrv.so

