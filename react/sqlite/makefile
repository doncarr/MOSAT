
export CPATH=../../include:..

RTCOMMON=../../librtcommon/librtcommon.a

CPP=g++ -g -Wall

all: main gen_sqlite react_def.db read_dat insert_scripts read_script out_obj.o

main: main.cpp 
	$(CPP) -o main -lsqlite3 main.cpp

gen_sqlite: gen_sqlite.cpp
	$(CPP) -o gen_sqlite gen_sqlite.cpp $(RTCOMMON) 

outmain.cpp write_one_fns.cpp out_obj.cpp: gen_sqlite
	./gen_sqlite ../dbfiles/

read_dat: read_dat.cpp
	$(CPP) -o read_dat read_dat.cpp $(RTCOMMON) 

react_def.db: gen_react_db
	rm -f react_def.db
	./gen_react_db

insert_scripts: insert_scripts.cpp
	$(CPP) -o insert_scripts insert_scripts.cpp -lsqlite3 $(RTCOMMON) 

read_script: read_script.cpp
	$(CPP) -o read_script read_script.cpp -lsqlite3 $(RTCOMMON)

gen_react_db: write_one_fns.cpp outmain.cpp insert_from_dat.cpp
	$(CPP) -o gen_react_db write_one_fns.cpp outmain.cpp insert_from_dat.cpp -lsqlite3 $(RTCOMMON)

out_obj.o: out_obj.cpp
	$(CPP) -c -o out_obj.o out_obj.cpp

clean: 
	rm -fv main gen_sqlite out_di out_ai out_di.cpp out_ai.cpp read_dat react_def.db write_one_fns.cpp outmain.cpp out_obj.cpp insert_scripts read_script gen_react_db
