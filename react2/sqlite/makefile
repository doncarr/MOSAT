
export CPATH=../../include:..

RTCOMMON=../../librtcommon/librtcommon.a

CPP=g++ -g -Wall

all: main gen_sqlite react_def.db read_dat insert_scripts read_script read_fns.o

main: main.cpp 
	$(CPP) -o main main.cpp -lsqlite3

gen_sqlite: gen_sqlite.cpp
	$(CPP) -o gen_sqlite gen_sqlite.cpp $(RTCOMMON) 

outmain.cpp write_one_fns.cpp read_fns.cpp: gen_sqlite
	./gen_sqlite ../dbfiles/

read_dat: read_dat.cpp
	$(CPP) -o read_dat read_dat.cpp $(RTCOMMON) 

react_def.db: gen_react_db
	rm -f react_def.db
	./gen_react_db

insert_scripts: insert_scripts.cpp
	$(CPP) -o insert_scripts insert_scripts.cpp $(RTCOMMON) -lsqlite3 

read_script: read_script.cpp
	$(CPP) -o read_script read_script.cpp $(RTCOMMON) -lsqlite3

gen_react_db: write_one_fns.cpp outmain.cpp insert_from_dat.cpp
	$(CPP) -o gen_react_db write_one_fns.cpp outmain.cpp insert_from_dat.cpp $(RTCOMMON) -lsqlite3

read_fns.o: read_fns.cpp
	$(CPP) -c -o read_fns.o read_fns.cpp

sqlite_read_fns.o: sqlite_read_fns.cpp
	$(CPP) -c -o sqlite_read_fns.o sqlite_read_fns.cpp

sql_read_apconfig.o: sql_read_apconfig.cpp
	$(CPP) -c -o sql_read_apconfig.o sql_read_apconfig.cpp

clean: 
	rm -fv main gen_sqlite out_di out_ai out_di.cpp out_ai.cpp read_dat react_def.db write_one_fns.cpp outmain.cpp read_fns.cpp insert_scripts read_script gen_react_db
