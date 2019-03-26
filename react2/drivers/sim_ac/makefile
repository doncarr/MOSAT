
export CPATH=../../../include:../../

libsimacdrv.so: sim_ac.o exp_sim.o ac_unit_sim.o random_tools.cpp sim_ac.h
	g++ -std=c++0x -Wall -fPIC -o libsimacdrv.so -shared sim_ac.o exp_sim.o ac_unit_sim.o random_tools.cpp

sim_ac.o: sim_ac.cpp
	g++ -std=c++0x -Wall -c -fPIC sim_ac.cpp

exp_sim.o: exp_sim.cpp
	g++ -std=c++0x -c -fPIC -o exp_sim.o exp_sim.cpp

ac_unit_sim.o: ac_unit_sim.cpp
	g++ -std=c++0x -c -fPIC -o ac_unit_sim.o ac_unit_sim.cpp

exp_sim: exp_sim.cpp exp_sim.h
	g++ -std=c++0x -Wall -o exp_sim exp_sim.cpp

unit: ac_unit_sim.cpp exp_sim.cpp random_tools.cpp
	g++ -std=c++0x -Wall -o unit ac_unit_sim.cpp exp_sim.cpp random_tools.cpp

clean:
	rm -v *.o *.so unit exp_sim

