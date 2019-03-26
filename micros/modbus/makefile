
export CPATH=../../include

RTCOMMON=../../librtcommon/librtcommon.a
RTMODBUS=../../modbus/librtmodbus.a

all: main crc.o mod_server_msg.o swap.o hexascii.o serial_mod_server client_server_test

hexascii.o: hexascii.c
	gcc -Wall -c hexascii.c

main.o: main.c
	gcc -Wall -c main.c

crc.o: crc.c 
	gcc -Wall -c crc.c 

mod_server_msg.o: mod_server_msg.c rt_modbus_opcodes.h 
	gcc -Wall -c mod_server_msg.c 

mod_server_test.o: mod_server_test.c
	gcc -Wall -c mod_server_test.c

swap.o: swap.c
	gcc -Wall -c swap.c

main: main.o mod_server_msg.o crc.o swap.o hexascii.o mod_server_test.o
	gcc -Wall -o main main.o mod_server_msg.o crc.o swap.o \
             hexascii.o mod_server_test.o

serial_mod_server.o: serial_mod_server.cpp
	g++ -Wall -c serial_mod_server.cpp

serial_mod_server: serial_mod_server.o  crc.o  hexascii.o mod_server_msg.o \
                 swap.o mod_server_test.o 
	g++ -o serial_mod_server serial_mod_server.o crc.o \
		 hexascii.o mod_server_msg.o  swap.o mod_server_test.o $(RTMODBUS)

client_server_test: client_server_test.o hexascii.o mod_server_msg.o \
                 swap.o mod_server_test.o mod_client_msg.o 
	g++ -o client_server_test client_server_test.o crc.o \
		 hexascii.o mod_server_msg.o  swap.o mod_server_test.o mod_client_msg.o 

clean: 
	rm -v *.o main serial_mod_server client_server_test
