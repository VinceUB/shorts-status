CFLAGS := $(shell curl-config --cflags) -g -Wall -Wextra
LDFLAGS := $(shell curl-config --libs) -lcjson 

all: shorts-status

shorts-status: main.c
	gcc -o $@ $^ ${CFLAGS} ${LDFLAGS}
