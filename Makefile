CFLAGS := $(shell curl-config --cflags)
LDFLAGS := $(shell curl-config --libs) -lcjson 

all: shorts-status

shorts-status: main.c
	gcc -o $@ $^ ${CFLAGS} ${LDFLAGS}
