VERSION=-D RAPIDBUS_VERSION=\"0.0.1\"
CC=clang-13
CFLAGS=-std=gnu99 -O0 -Wall -Wextra -Wpedantic -pedantic -pedantic-errors
LDFLAGS=-lrt -lpaho-mqtt3a

.ONESHELL:
.SHELLFLAGS += -e
.PHONY: clean check

all: clean rapidbusd

rapidbusd:
	$(CC) $(CFLAGS) $(VERSION) -o rapidbusd rapidbusd.c config.c timers.c modbus.c mqtt.c strlcpy.c $(LDFLAGS)

clean:
	rm -f rapidbusd rapidbusd.o
	echo "Clean done"

run: all
	./rapidbusd -c ./rapidbusd.conf.example

check:
	cppcheck *.c *.h
	clang-format -i *.c *.h

install:
	cp rapidbusd.conf.example /etc/rapidbusd.conf
	cp rapidbusd /usr/local/bin

uninstall:
	mv /etc/rapidbusd.conf /etc/rapidbusd.conf.old
	rm -f /usr/local/bin/rapidbusd
