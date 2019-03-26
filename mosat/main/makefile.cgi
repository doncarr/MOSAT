
export CPATH=../../include

CC= gcc -Wall
DIR=cgi-bin/

all:	$(DIR)interdel.cpp  $(DIR)interdet.cpp  $(DIR)interrupt1.cpp  $(DIR)interrupt2.cpp


$(DIR)interdel: interdel.cpp
	$(CC) -o $(DIR)interdel interdel.cpp

$(DIR)interdel: interdet.cpp
	$(CC) -o $(DIR)interdet interdet.cpp

$(DIR)interdel: interrupt1.cpp
	$(CC) -o $(DIR)interrupt1 interrupt1.cpp

$(DIR)interdel: interrupt2.cpp
	$(CC) -o $(DIR)interrupt2 interrupt2.cpp
#$(DIR)oem_iodb_cpp.o:	oem_iodb_cpp.cpp
#	$(CC) -o $(DIR)oem_iodb_cpp.o -c -g -Wall oem_iodb_cpp.cpp

#$(DIR)oem_iodb_cpp: $(DIR)oem_iodb_cpp.o 
#	$(CC) -o $(DIR)oem_iodb_cpp  $(DIR)oem_iodb_cpp.o -lsxiodb -lsxio

#$(DIR)ppc_thr:        thread.cpp
#	$(CC) -lpthread -o $(DIR)ppc_thr thread.cpp 

#$(DIR)dump_events:        $(DIR)dump_events.o
#	$(CC) -lpthread -o $(DIR)dump_events $(DIR)dump_events.o

clean: 
	rm -fv $(DIR)interdel.cpp  $(DIR)interdet.cpp  $(DIR)interrupt1.cpp  $(DIR)interrupt2.cpp 
