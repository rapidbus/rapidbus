## Preparing development environment

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
