
export CPATH=../../../include:../../

libsimpumpdrv.so: simpumpdrv.o simpumpdrv.h
	g++ -Wall -o libsimpumpdrv.so -shared simpumpdrv.o 

simpumpdrv.o: simpumpdrv.cpp
	g++ -Wall -c -fPIC simpumpdrv.cpp

clean:
	rm *.o *.so

