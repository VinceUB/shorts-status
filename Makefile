CFLAGS := $(shell curl-config --cflags) -g -Wall -Wextra
LDFLAGS := $(shell curl-config --libs) -lcjson 

SRCS := src/main.c
OBJS := $(patsubst src/%.c,obj/%.o,${SRCS})

pam_shorts.so: CFLAGS += -DPAM_SUPPORT -fPIC

obj/%.o: src/%.c | obj
	gcc -c -o $@ $^ ${CFLAGS}

pam_shorts.so: ${OBJS}
	gcc -shared -o $@ $^ ${LDFLAGS}

shorts: ${OBJS}
	gcc -o $@ $^ ${LDFLAGS}

obj:
	mkdir obj
