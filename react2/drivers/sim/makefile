
export CPATH=../../../include:../../

libsimdriver.so: simdriver.cpp 
	g++ -fPIC -std=c++0x -o libsimdriver.so -shared simdriver.cpp

clean:
	rm -fv libsimdriver.so

