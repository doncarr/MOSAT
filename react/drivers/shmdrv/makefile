
export CPATH=../../../include:../../

libreactshmdrv.so: reactshmdrv.o reactshmio.o
	g++ -o libreactshmdrv.so -shared reactshmdrv.o reactshmio.o 

reactshmdrv.o:	reactshmdrv.cpp reactshmdrv.h
	g++ -fPIC -c reactshmdrv.cpp 

reactshmio.o:	../../reactshmio.cpp ../../reactshmio.h
	g++ -fPIC -c -o reactshmio.o ../../reactshmio.cpp 

clean:
	rm *.o *.so


