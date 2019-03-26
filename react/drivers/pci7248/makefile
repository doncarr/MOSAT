export CPATH=../../../include:../../

libpci7248.so: pci7248.o
	g++ -o libpci7248.so -shared pci7248.o

pci7248.o: pci7248.cpp pci7248.h
	g++ -c -fPIC pci7248.cpp
                                                                                                                             
clean:
	rm *.o *.so


