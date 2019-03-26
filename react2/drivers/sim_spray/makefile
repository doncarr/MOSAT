
.PHONY: clean

export CPATH=../../../include:../../

spraydrv.so: spraydrv.cpp
	g++ -fPIC -std=c++0x -o spraydrv.so -shared spraydrv.cpp

clean:
	rm  *.so

