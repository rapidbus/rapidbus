CC=gcc
#CFLAGS=-L. -std=c99

.PHONY: clean check

all: clean rapidbusd

rapidbusd: rapidbusd
	$(CC) $(CFLAGS) -o rapidbusd rapidbusd.c config.c timers.c modbus.c mqtt.c -Wall -lrt -lpaho-mqtt3a

clean:
	rm -f rapidbusd rapidbusd.o
	echo "Clean done"

run: all
	./rapidbusd

check:
	cppcheck *.c *.h
	clang-format -i *.c *.h
