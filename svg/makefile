
export CPATH=../include/

.PHONY : clean
.PHONY : all
.PHONY : so_files 
.PHONY : doc 
.PHONY : faceplates
.PHONY: imasa gauge spray.slider slider svgs feed cool switch tina transfer spray

RTCOMMON=

SILENT=-silent
#SILENT=
GENMODE=-sim
#GENMODE=-react
TINAMODE=-sim_file sim_tina.js
#TINAMODE=-react

CPP=g++ -Wall -Werror -Wformat=2 -std=c++0x

all: so_files gen_points gen_bar gen_tank gen_display svgs doc


so_files: gen_so_makefile
	./gen_so_makefile
	cd plugins; make; cd ..

gen_points: gen_points.cpp
	$(CPP) -o gen_points gen_points.cpp

gen_bar: gen_bar.cpp gen_logo.cpp scales.cpp include_file.cpp
	$(CPP) -o gen_bar gen_bar.cpp gen_logo.cpp scales.cpp include_file.cpp

gen_so_makefile: gen_so_makefile.cpp
	$(CPP) -o gen_so_makefile gen_so_makefile.cpp

gen_tank: gen_tank.cpp
	$(CPP) -o gen_tank gen_tank.cpp

gen_display:  gen_display.cpp doc_text.cpp doc_html.cpp include_file.cpp gen_display.h react_svg.h
	$(CPP) gen_display.cpp doc_text.cpp doc_html.cpp include_file.cpp -ldl -rdynamic -o gen_display $(RTCOMMON)

dtest.svg: display.txt gen_display so_files
	./gen_display $(SILENT) -f display.txt -d ./plugins/ -o dtest.svg $(GENMODE)

install: 
	mkdir /usr/local/etc/react2/svg
	cp feed.svg cool.svg spray.svg transfer.svg tina.svg switch.svg imasa.svg pipes.svg pipes2d.svg gauge.svg slider.svg /usr/local/etc/react2/svg/
	cp svgdoc.html /usr/local/etc/react2/svg/
	cp svgdoc.txt /usr/local/etc/react2/svg/

uninstall:
	rm -rf /usr/local/etc/react2/svg

depend:
	gcc -MM -MF depend gen_display.cpp


gauge:
	./gen_display $(SILENT) -f display.gauge.txt -o gauge.svg $(GENMODE)
#	cp display.gauge.txt gauge.svg demofiles

spray.slider:
	./gen_display $(SILENT) -f display.spray.slider.txt -o spray.slider.svg $(GENMODE)
#	cp display.spray.slider.txt spray.slider.svg demofiles

slider:
	./gen_display $(SILENT) -f display.slider.txt -o slider.svg $(GENMODE)

doc:
	./gen_display $(SILENT) -doc -html -o svgdoc.html
	./gen_display $(SILENT) -doc -text -o svgdoc.txt

feed: 
	./gen_display $(SILENT) -f display.feedtanks.txt -o feed.svg $(GENMODE) -popup
#	cp display.feedtanks.txt feed.svg demofiles

cool:
	./gen_display $(SILENT) -f display.coolingtower.txt -o cool.svg $(GENMODE) -popup
#	cp display.coolingtower.txt cool.svg demofiles

switch:
	./gen_display $(SILENT) -f display.switch.txt -o switch.svg $(GENMODE)
#	cp display.switch.txt switch.svg demofiles

spray:
	./gen_display $(SILENT) -f display.spray.round.txt -o spray.svg $(GENMODE) -popup
#	cp display.spray.round.txt spray.svg demofiles

tina:
	./gen_display $(SILENT) -f display.tina.txt -o tina.svg $(TINAMODE) -popup
#	cp display.tina.txt tina.svg demofiles

transfer:
	./gen_display $(SILENT) -f display.transfer.txt -o transfer.svg $(GENMODE)
#	cp display.transfer.txt transfer.svg demofiles

imasa:
	./gen_display $(SILENT) -f display.imasa.txt -o imasa.svg $(GENMODE)

pipes:
	./gen_display $(SILENT) -f display.pipes.txt -o pipes.svg $(GENMODE)
#	cp display.pipes.txt pipes.svg demofiles

pipes2d:
	./gen_display $(SILENT) -f display.pipes2d.txt -o pipes2d.svg $(GENMODE)
#	cp display.pipes2d.txt pipes2d.svg demofiles

dash:
	./gen_display $(SILENT) -f dash.txt -o dash.svg -sim 


faceplates:
	./gen_display $(SILENT) -f display.aibar.txt -o aibar.svg $(GENMODE)
	./gen_display $(SILENT) -f display.airound.txt -o airound.svg $(GENMODE)
	./gen_display $(SILENT) -f display.aineedle.txt  -o aineedle.svg $(GENMODE)
	./gen_display $(SILENT) -f display.aoround.txt -o aoround.svg $(GENMODE)
	./gen_display $(SILENT) -f display.di_rect.txt  -o di_rect.svg $(GENMODE)
	./gen_display $(SILENT) -f display.dopopup.txt  -o dopopup.svg $(GENMODE) -popup
	./gen_display $(SILENT) -f display.do_switch.txt -o doswitch.svg $(GENMODE)


clean:
	rm -rv gen_points gen_bar gen_tank gen_so_makefile ./plugins/*.so gen_display

svgs: feed cool spray transfer tina switch imasa pipes pipes2d gauge slider
#	google-chrome demofiles/gauge.svg

