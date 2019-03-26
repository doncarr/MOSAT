
DIRS= librtcommon modbus mosat react2 svg trainpos onboard classic_tcontrol

PPCDIRS= librtcommon mosat react 

ARMDIRS= librtcommon modbus onboard react

MAKE= make

.PHONY : clean all 

all:
	for i in $(DIRS); do cd "$$i"; $(MAKE); cd ..; done

clean:
	for i in $(DIRS); do cd "$$i"; $(MAKE) clean; cd ..; done

tp:
	cd librtcommon; $(MAKE); cd ..
	cd trainpos; $(MAKE); cd ..

ppcall:
	for i in $(PPCDIRS); do cd "$$i"; $(MAKE) ppcall; cd ..; done

ppcclean:
	for i in $(PPCDIRS); do cd "$$i"; $(MAKE) ppcclean; cd ..; done

armall:
	for i in $(ARMDIRS); do cd "$$i"; $(MAKE) armall; cd ..; done

armclean:
	for i in $(ARMDIRS); do cd "$$i"; $(MAKE) armclean; cd ..; done
