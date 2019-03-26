
export CPATH=../../../include:../../

libsimfiledriver.so: simfiledriver.o simfiledriver.h
	g++ -std=c++0x -o libsimfiledriver.so -shared simfiledriver.o

simfiledriver.o: simfiledriver.cpp
	g++ -std=c++0x -c -fPIC simfiledriver.cpp

clean:
	rm *.o *.so

