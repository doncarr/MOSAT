
export CPATH=../../../include:../../

libsimplasticdrv.so: simplasticdrv.o simplasticdrv.h
	g++ -fPIC -std=c++0x -Wall -o libsimplasticdrv.so -shared simplasticdrv.o 

simplasticdrv.o: simplasticdrv.cpp
	g++ -fPIC -std=c++0x -Wall -c simplasticdrv.cpp

clean:
	rm *.o *.so

