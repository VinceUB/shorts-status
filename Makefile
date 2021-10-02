CFLAGS := $(shell curl-config --cflags) -g -Wall -Wextra
LDFLAGS := $(shell curl-config --libs) -lcjson 

pam_shorts.so: main.c #TODO: Clean this shit up
	gcc -c -o main.o $^ ${CFLAGS} -DPAM_SUPPORT -fPIC
	gcc -shared -o $@ main.o ${LDFLAGS}
	rm main.o

shorts-status: main.c
	gcc -o $@ $^ ${CFLAGS} ${LDFLAGS}
