CC=clang-11
CFLAGS=-std=gnu99 -O3 -Wall -Wextra -pedantic
LDFLAGS=-lrt -lpaho-mqtt3a -lbsd

.PHONY: clean check

all: clean rapidbusd

rapidbusd:
	$(CC) $(CFLAGS) -o rapidbusd rapidbusd.c config.c timers.c modbus.c mqtt.c $(LDFLAGS)

clean:
	rm -f rapidbusd rapidbusd.o
	echo "Clean done"

run: all
	./rapidbusd

check:
	cppcheck *.c *.h
	clang-format -i *.c *.h

install:
	cp rapidbusd.conf.example /etc/rapidbusd.conf
	cp rapidbusd /usr/local/bin
