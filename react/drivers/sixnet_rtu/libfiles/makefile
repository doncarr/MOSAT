

CPP=powerpc-linux-g++

libsx.so:
	$(CPP) -shared -o libsx.so cfgparser.o client_inter.o client_io.o client_shmem.o client_supp.o \
			client_utils.o ddio.o ddmisc.o iodb.o mgr_utils.o taginfo.o
