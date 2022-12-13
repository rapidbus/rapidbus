VERSION = $(shell cat version.txt)
VERSION_FLAG=-D RAPIDBUS_VERSION=\"$(VERSION)\"

#CC=gcc

CFLAGS=-std=gnu99 -O0 -Wall -Wextra -Wpedantic -pedantic -pedantic-errors -Werror \
      -Wformat=2 -Wno-unused-parameter -Wshadow \
      -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
      -Wnested-externs -Wmissing-include-dirs -Wredundant-decls \
      -fpie -fpic -g3 -fstack-protector-strong -grecord-gcc-switches \
      -Werror=format-security -Werror=implicit-function-declaration \
      -Wmisleading-indentation
# -fno-omit-frame-pointer -fsanitize=address # fails on Alpine linux
# -D_FORTIFY_SOURCE=2 # fails on RHEL8

LDFLAGS=-lpthread -lrt -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu -l:libpaho-mqtt3a.a

.ONESHELL:
.SHELLFLAGS += -e
.PHONY: clean check

all: clean rapidbusd

rapidbusd:
	$(CC) $(CFLAGS) $(VERSION_FLAG) -o rapidbusd rapidbusd.c config.c timers.c modbus.c mqtt.c strlcpy.c $(LDFLAGS)

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

build_release:
	for DISTRO in alpine3 debian10 debian11 rhel8 rhel9; do \
		docker build -f Dockerfile-$$DISTRO -t rapidbus/build-$$DISTRO . ; \
		docker run -it -v $(shell pwd):/opt/rapidbus --entrypoint /bin/sh rapidbus/build-$$DISTRO -c "cd /opt/rapidbus/; make all; ./rapidbusd -h" ; \
		rm -f rapidbusd-$$VERSION_$$DISTRO.tar.gz ; \
		tar czfv rapidbusd-$$VERSION_$$DISTRO.tar.gz rapidbusd rapidbusd.conf.example ; \
	done