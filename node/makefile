# Makefile for react.

all:

install:
	mkdir -p /usr/local/etc/react2/web
	cp webserver.js /usr/local/etc/react2/web/

uninstall:
	rm -rf /usr/local/etc/react2/web

install-ubuntu-inits:
	cp inits/ubuntu/init/react2web.conf /etc/init/
	cp inits/ubuntu/init.d/react2web /etc/init.d/
	chmod a+x /etc/init.d/react2web
	update-rc.d react2web defaults

uninstall-ubuntu-inits:
	update-rc.d react2web remove
	rm /etc/init/react2web.conf
	rm /etc/init.d/react2web

