CC=cc
CFLAGS=-I.

.PHONY: clean check

all: clean check rapidbusd

rapidbusd: rapidbusd
	$(CC) -o rapidbusd rapidbusd.c timers.c modbus.c mqtt.c -Wall -lrt -lpaho-mqtt3a

clean:
	rm -f rapidbusd rapidbusd.o
	echo "Clean done"

run: all
	./rapidbusd

check:
	cppcheck *.c *.h
	clang-format -i *.c *.h

time: timertest.o
	$(CC) -o timertest timertest.c -lrt
