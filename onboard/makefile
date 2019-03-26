
export CPATH=../include

CPP=g++ -g -Wall -Werror

INCLUDES= d2x24.h frtimer.h odo_mode.h onboard.h peekpoke.h profile_reader.h rt_serial.h sim_reader.h\
	 spd_algo_DC.h spd_algo.h spd_algo_RD.h spd_algo_VV.h spd_comm.h spd_display.h tsarm.h ts_keypad.h


RTCOMMON=../librtcommon/librtcommon.a
RTMODBUS=../modbus/librtmodbus.a

%.o:    %.cpp $(INCLUDES)
	$(CPP) -c -o $@ -Wall $<

%.o:    %.c $(INCLUDES)
	$(CC) -c -o $@ -Wall $<

all: onboard onboard_2x24 onboard_4x40 drive odo_mode_4x40 odo_mode sim_CBTC CBTC_zone


onboard:	onboard.o spd_display.o spd_comm.o spd_select.o spd_algo_DC.o spd_algo_RD.o spd_algo_VV.o \
		profile_reader.o  sim_reader.o alg_compare.o $(INCLUDES)
	$(CPP) -o onboard onboard.o spd_display.o spd_comm.o spd_select.o sim_reader.o alg_compare.o \
			spd_algo_DC.o spd_algo_RD.o  spd_algo_VV.o profile_reader.o  $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

sim_CBTC:	sim_CBTC.o spd_display.o spd_comm.o spd_select.o spd_algo_DC.o spd_algo_RD.o spd_algo_VV.o \
		profile_reader.o  sim_reader.o alg_compare.o $(INCLUDES)
	$(CPP) -o sim_CBTC sim_CBTC.o spd_display.o spd_comm.o spd_select.o sim_reader.o alg_compare.o \
			spd_algo_DC.o spd_algo_RD.o  spd_algo_VV.o profile_reader.o  $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

onboard_4x40:	onboard.o spd_display_4x40.o spd_comm.o spd_select.o spd_algo_DC.o spd_algo_RD.o spd_algo_VV.o \
		profile_reader.o  sim_reader.o alg_compare.o $(INCLUDES)
	$(CPP) -o onboard_4x40 onboard.o spd_display_4x40.o spd_comm.o spd_select.o sim_reader.o alg_compare.o \
			spd_algo_DC.o spd_algo_RD.o  spd_algo_VV.o profile_reader.o -lpthread $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

drive:	onboard.o spd_display.o spd_comm_drive.o spd_select.o spd_algo_DC.o spd_algo_RD.o spd_algo_VV.o \
		profile_reader.o  sim_reader.o alg_compare.o $(INCLUDES)
	$(CPP) -o drive onboard.o spd_display.o spd_comm_drive.o spd_select.o sim_reader.o alg_compare.o \
			spd_algo_DC.o spd_algo_RD.o  spd_algo_VV.o profile_reader.o $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

onboard_2x24:	onboard.o spd_display_2x24.o spd_comm.o d2x24_curses.o spd_select.o sim_reader.o \
		spd_algo_DC.o spd_algo_RD.o  spd_algo_VV.o profile_reader.o alg_compare.o $(INCLUDES)
	$(CPP) -o onboard_2x24 onboard.o spd_display_2x24.o spd_comm.o sim_reader.o profile_reader.o alg_compare.o \
		d2x24_curses.o spd_select.o  spd_algo_DC.o spd_algo_RD.o  spd_algo_VV.o $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

lcdmesg: lcdmesg.c
	gcc -Wall -Werror -o lcdmesg lcdmesg.c

test_both:        test_both.o -lpthread d2x24_curses.o
	$(CC) -o test_both -lpthread -lcurses test_both.o d2x24_curses.o $(RTCOMMON)

keypad: keypad.c
	gcc -o keypad keypad.c

check_speed:	check_speed.o
	$(CPP) -o  check_speed check_speed.o -lncurses -lrt

mo_test: mo_test.cpp
	$(CPP) -o mo_test mo_test.cpp  $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

autonomous_mode: autonomous_mode.o spd_comm.o make_profile_dir.o 
	$(CPP) -o autonomous_mode autonomous_mode.o spd_comm.o make_profile_dir.o  \
	  $(RTMODBUS) $(RTCOMMON) -lrt -lncurses

odo_mode: odo_mode.o odo_display.o spd_comm.o $(INCLUDES)
	$(CPP) -o odo_mode odo_mode.o spd_comm.o \
	  odo_display.o $(RTMODBUS) $(RTCOMMON) -lrt -lncurses -lpthread

odo_mode_4x40: odo_mode.o odo_display_4x40.o spd_comm.o 
	$(CPP) -o odo_mode_4x40 -DUSE_PRINTF odo_mode.o spd_comm.o \
	  odo_display_4x40.o $(RTMODBUS) $(RTCOMMON) -lrt -lncurses -lpthread

#profile_reader: profile_reader.cpp profile_reader.h
#	$(CPP) -o profile_reader profile_reader.cpp  $(RTCOMMON)
		
clean:
	rm -fv $(DIR)*.o
	rm -fv $(DIR)onboard
	rm -fv $(DIR)drive
	rm -fv $(DIR)onboard_2x24
	rm -fv $(DIR)lcdmesg
	rm -fv $(DIR)keypad
	rm -fv $(DIR)onboard_4x40
	rm -fv $(DIR)odo_mode_4x40
	rm -fv $(DIR)odo_mode
	rm -fv sim_CBTC
	rm -fv CBTC_zone

armall:
	make -f makefile.arm

armclean:
	make -f makefile.arm clean

