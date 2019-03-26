
DIRS= main

MAKE= make

MAKEPPC = make -f makefile.ppc

all:
	for i in $(DIRS); do cd "$$i"; $(MAKE); cd ..; done

ppcall:
	for i in $(DIRS); do cd "$$i"; $(MAKEPPC); cd ..; done

ppcclean:
	for i in $(DIRS); do cd "$$i"; $(MAKEPPC) clean; cd ..; done
	

clean:
	for i in $(DIRS); do cd "$$i"; $(MAKE) clean; $(MAKEPPC) clean; cd ..; done
