

all: gen_points gen_bar gen_tank hash

hash: hash.cpp
	g++ -std=c++0x -Wall -o hash hash.cpp

gen_points: gen_points.cpp
	g++ -g -std=c++0x -Wall -o gen_points gen_points.cpp

gen_bar: gen_bar.cpp gen_logo.cpp scales.cpp
	g++ -g -Wall -o gen_bar gen_bar.cpp gen_logo.cpp scales.cpp

gen_tank: gen_tank.cpp
	g++ -g -Wall -o gen_tank gen_tank.cpp

hash: hash.cpp
	g++ -std=c++0x -o hash hash.cpp

install: 
	sudo cp ai.svg /var/www

clean:
	rm -rv gen_points gen_bar gen_tank hash 
