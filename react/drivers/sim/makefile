
export CPATH=../../../include:../../

libsimdriver.so: simdriver.o burnersim.o simiodriver.h
	g++ -o libsimdriver.so -shared simdriver.o burnersim.o

simdriver.o: simdriver.cpp
	g++ -c -fPIC simdriver.cpp

burnersim.o: burnersim.cpp
	g++ -c -fPIC burnersim.cpp

clean:
	rm *.o *.so

