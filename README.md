# RapidBus

<a href="https://scan.coverity.com/projects/rapidbus-rapidbus">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/27066/badge.svg"/>
</a>

[![CodeQL](https://github.com/rapidbus/rapidbus/actions/workflows/codeql.yml/badge.svg)](https://github.com/rapidbus/rapidbus/actions/workflows/codeql.yml)

RapidBus is a bridging application between Operational Technologies and Information Technologies (OT/IT) connecting serial-enabled sensors to MQTT broker. Essentially forwarding metering data from serial interface over MODBUS RTU to MQTT broker.

This program is written in C (clang / gnu99 (ISO C 1999 with GNU extensions)) and targeted to be used on Linux platforms on the edge of Industry 4.0 networks. For detailed documentation see here: https://www.rapidbus.org

## Preparing development environment

### Ubuntu

Ubuntu pre-impish (Ubuntu 21.10) not supported they dont include package https://packages.ubuntu.com/impish/libpaho-mqtt-dev package

#### Ubuntu 21.10

```
apt install libpaho-mqtt-dev libpaho-mqtt1.3 libbsd-dev
```

#### Ubuntu 22.04

```
apt install clang-tools-11 libpaho-mqtt-dev libpaho-mqtt1.3 libbsd-dev socat
# create virtual serial interface
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# in another terminal
cat < /dev/pts/2
```

### RHEL

#### RHEL 7

```
yum install cppcheck rpm-build
# install paho-c from EPEL?
git clone https://github.com/eclipse/paho.mqtt.c.git
cd org.eclipse.paho.mqtt.c.git
make
sudo make install
```
