
export CPATH=../../../include:../../

libsimacdrv.so: sim_ac.o exp_sim.o ac_unit_sim.o random_tools.o sim_ac.h
	g++ -Wall -o libsimacdrv.so -shared sim_ac.o exp_sim.o ac_unit_sim.o random_tools.o

simac.o: sim_ac.cpp
	g++ -Wall -c -fPIC sim_ac.cpp

exp_sim: exp_sim.cpp exp_sim.h
	g++ -Wall -o exp_sim exp_sim.cpp

unit: ac_unit_sim.cpp exp_sim.cpp random_tools.cpp
	g++ -Wall -o unit ac_unit_sim.cpp exp_sim.cpp random_tools.cpp

clean:
	rm -v *.o *.so unit exp_sim

