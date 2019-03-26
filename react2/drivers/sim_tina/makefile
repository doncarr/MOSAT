
export CPATH=../../../include:../../

libsimtinadrv.so: simtinadrv.cpp 
	g++ -std=c++0x -Wall -fPIC -o libsimtinadrv.so -shared simtinadrv.cpp

clean:
	rm -fv libsimtinadrv.so

