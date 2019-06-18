

all: xjson_test

CC=gcc -g -Wall -std=c99

xjson_test: xjson.c xjson.h xjson_test.c
	$(CC) -o xjson_test xjson_test.c xjson.c

clean:
	rm -fv *.o xjson_test
