VERSION=-D RAPIDBUS_VERSION=\"0.1.1\"
#CC=clang-13
CFLAGS=-v -std=gnu99 -O0 -Wall -Wextra -Wpedantic -pedantic -pedantic-errors
LDFLAGS=-lpthread /usr/local/lib64/libpaho-mqtt3a.a -lrt

.ONESHELL:
.SHELLFLAGS += -e
.PHONY: clean check

all: clean rapidbusd

rapidbusd:
	$(CC) $(CFLAGS) $(VERSION) -o rapidbusd rapidbusd.c config.c timers.c modbus.c mqtt.c strlcpy.c $(LDFLAGS)

clean:
	rm -f rapidbusd rapidbusd.o
	echo "Clean done"

run:
	./build-debian11.sh
	./rapidbusd -c ./rapidbusd.conf.example

check:
	cppcheck *.c *.h
	clang-format -i *.c *.h
#	scan-build-13 --status-bugs make

install:
	cp rapidbusd.conf.example /etc/rapidbusd.conf
	cp rapidbusd /usr/local/bin

uninstall:
	mv /etc/rapidbusd.conf /etc/rapidbusd.conf.old
	rm -f /usr/local/bin/rapidbusd
