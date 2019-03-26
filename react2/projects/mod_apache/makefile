

.PHONY: mod install restart clean

react:
	apxs2 -c mod_react.c

helloworld: 
	apxs2 -c mod_helloworld.c

install:
	sudo apxs2 -ie mod_react.la
#	sudo apxs2 -ie mod_helloworld.la

restart:
	sudo /etc/init.d/apache2 restart

stop:
	sudo /etc/init.d/apache2 stop 


clean:
	rm -rfv mod_helloworld.la mod_helloworld.lo mod_helloworld.slo .libs 
	rm -rfv mod_react.la mod_react.lo mod_react.slo .libs 
