# RapidBus

<a href="https://scan.coverity.com/projects/rapidbus-rapidbus">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/27066/badge.svg"/>
</a>

RapidBus is a bridging application between Operational Technoologies and Information Technologies (OT/IT) connecting serial-enabled sensors to MQTT broker. Essentially forwarding metering data from serial interface over MODBUS RTU to MQTT broker.

Its written in C and targeted to be used on Linux platforms on the edge of Industry 4.0 networks. For detailed documentation see here: https://rapidbus.github.io/rapidbus/

## Preparing development environment

If you are interested in code development read further.

### Debian

Debian pre-bullseye (Debian 11) not supported as they dont include package https://packages.debian.org/bullseye/libpaho-mqtt-dev

#### Debian 11

```
apt install libpaho-mqtt-dev libpaho-mqtt1.3
```

### Ubuntu

Ubuntu pre-impish (Ubuntu 21.10) not supported they dont include package https://packages.ubuntu.com/impish/libpaho-mqtt-dev package

#### Ubuntu 21.10

```
apt install libpaho-mqtt-dev libpaho-mqtt1.3
```

#### Ubuntu 22.04

```
apt install clang-tools-11 libpaho-mqtt-dev libpaho-mqtt1.3
```

### RHEL

#### RHEL 7

```
yum install cppcheck rpm-build
git clone https://github.com/eclipse/paho.mqtt.c.git
cd org.eclipse.paho.mqtt.c.git
make
sudo make install
```
