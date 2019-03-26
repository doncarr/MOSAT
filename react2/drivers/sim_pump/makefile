
export CPATH=../../../include:../../

libsimpumpdrv.so: simpumpdrv.o simpumpdrv.h
	g++ -fPIC -std=c++0x -o libsimpumpdrv.so -shared simpumpdrv.o 

simpumpdrv.o: simpumpdrv.cpp
	g++ -fPIC -std=c++0x -c simpumpdrv.cpp

clean:
	rm *.o *.so

