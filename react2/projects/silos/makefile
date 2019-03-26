
export CPATH=../../../include

RTCOMMON=../../../librtcommon/librtcommon.a

all: temp weblogger divide webbackground gen_graph dump_dat read_sim_file gen_react \
	temp_graph col_graph test control_loop

temp: temp.cpp  
	g++ -Wall -o temp temp.cpp  
#	../../pl out_temp_hum_sim.txt 

weblogger: weblogger.cpp
	g++ -Wall -o weblogger weblogger.cpp -lrt

webbackground: webbackground.cpp
	g++ -Wall -o webbackground webbackground.cpp $(RTCOMMON) -lrt

divide: divide.cpp
	g++ -Wall -g -o divide divide.cpp

gen_graph: gen_graph.cpp
	g++ -Wall -o gen_graph gen_graph.cpp

dump_dat: dump_dat.cpp
	g++ -Wall -o dump_dat dump_dat.cpp

read_sim_file: read_sim_file.cpp
	g++ -Wall -o read_sim_file read_sim_file.cpp

gen_react: gen_react.cpp
	g++ -Wall -o gen_react gen_react.cpp

col_graph: col_graph.cpp
	g++ -Wall -o col_graph col_graph.cpp

temp_graph: temp_graph.cpp
	g++ -Wall -o temp_graph temp_graph.cpp

test: test_post_cmds.cpp
	g++ -Wall -o test test_post_cmds.cpp 


control_loop: control_loop.cpp
	g++ -o control_loop control_loop.cpp

clean:
	rm -vf temp weblogger divide webbackground gen_graph read_sim_file gen_react test control_loop temp_graph col_graph dump_dat

