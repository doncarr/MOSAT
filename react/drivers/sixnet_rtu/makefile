
export CPATH=../../../include:../..

CPP=powerpc-linux-g++ -Wall

libsixnet_rtu.so: sixnet_rtu.o 
	$(CPP) -o libsixnet_rtu.so -shared sixnet_rtu.o 

#/opt/sixnet/powerpc-linux/lib/libsxio.a  /opt/sixnet/powerpc-linux/lib/libsxiodb.a 

sixnet_rtu.o:	sixnet_rtu.cpp
	$(CPP) -fPIC -c sixnet_rtu.cpp 

clean:
	rm *.o *.so

