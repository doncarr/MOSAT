
export CPATH=../include

CPP=g++ -Wall -Werror

INCLUDES=tcontrol.h alg1.h train_sim.h event_alg.h dc_evalg.h gp_evalg.h ri_evalg.h ag_evalg.h sim_msg.h trainpos.h ri_tplib.h ri_adtlib.h

RTCOMMON=../librtcommon/librtcommon.a

%.o:    %.cpp $(INCLUDES)
	$(CPP) -c -o $@ $<

all: trainpos line_sim sim_error disp_curses rtautogen

trainpos: trainpos.o event_dispatch.o alg1.o alarm_notify.o alarm_admin.o \
      msg_queue.o dc_evalg.o gp_evalg.o ag_evalg.o ri_evalg.o ri_tplib.o time_table.o \
      display_reader.o section_reader.o gen_displays.o $(INCLUDES) 
	$(CPP) -o trainpos trainpos.o event_dispatch.o alg1.o \
                 alarm_notify.o alarm_admin.o msg_queue.o dc_evalg.o \
                 gp_evalg.o ag_evalg.o ri_evalg.o ri_tplib.o time_table.o display_reader.o \
		section_reader.o gen_displays.o $(RTCOMMON) -lncurses

line_sim: line_sim.o msg_queue.o train_sim.o sim_msg.o section_reader.o $(INCLUDES)
	$(CPP) -o line_sim line_sim.o msg_queue.o \
               train_sim.o sim_msg.o section_reader.o $(RTCOMMON)  -lncurses

sim_error: sim_error.o $(INCLUDES)
	$(CPP) -o sim_error sim_error.o sim_msg.o

disp_curses: disp_curses.cpp
	$(CPP) -o disp_curses disp_curses.cpp -lncurses
clean:
	rm -fv trainpos line_sim sim_error disp_curses *.o rtautogen

rtautogen:	rtautogen.cpp
	g++ -Wall -o rtautogen rtautogen.cpp $(RTCOMMON) -lncurses

check_section_file: check_section_file.cpp  section_reader.o $(INCLUDES)
	g++ -Wall -o check_section_file check_section_file.cpp section_reader.o $(RTCOMMON)  -lncurses

n_x: n_section_reader.cpp n_section_reader.h
	g++ -Wall -o n_x n_section_reader.cpp $(RTCOMMON) -lncurses

add_column: add_column.cpp
	g++ -Wall -o add_column add_column.cpp $(RTCOMMON) -lncurses
