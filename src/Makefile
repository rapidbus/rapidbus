VERSION=-D RAPIDBUS_VERSION=\"0.1.2\"

#CC=gcc

CFLAGS=-std=gnu99 -O0 -Wall -Wextra -Wpedantic -pedantic -pedantic-errors -Werror \
      -Wformat=2 -Wno-unused-parameter -Wshadow \
      -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
      -Wnested-externs -Wmissing-include-dirs -Wredundant-decls \
      -D_FORTIFY_SOURCE=2 -fpie -fpic -g3 -fstack-protector-strong -grecord-gcc-switches \
      -Werror=format-security -Werror=implicit-function-declaration \
      -Wmisleading-indentation -fno-omit-frame-pointer -fsanitize=address

LDFLAGS=-lpthread -lrt -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu -l:libpaho-mqtt3a.a

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
