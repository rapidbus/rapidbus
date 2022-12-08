# Bridge to forward data from physical sensors to MQTT

RapidBus is a bridge to collect data from MODBUS (RTU) enabled sensors or PLCs and rapidly forward them to MQTT broker for further analysis.

RapidBus is used in OT/IT environments where its necessary to bridge the physical sensors with IT infrastructure. RapidBus is usually configured to read data in high frequency from multiple sensors on one physical serial interface (RS-485 for example) using RTU MODBUS protocol and then forward this data interpreted to MQTT topic - further analyzed by tools such as Grafana, Red-Node or stored in databases such as InfluxDB.

By running multiple instances of RapidBus daemon it is possible to manage multiple physical serial interfaces on one edge system in Industry 4.0 / IIOT architecture.

## Support

Official support is provided via GitHub issues on https://github.com/rapidbus/rapidbus/issues only for now. If you need commercial guarenteed support please ask via info@moirelabs.com or via GitHub issues.

## Installation

RapidBus is distributed as a single compiled binary file for Linux systems. It is not, and will not be supported on other platforms, because it is very tightly coupled with serial drivers in Linux. This strong coupling enables the high performance of RapidBus and straightforward maintenance, development, testing and support.

The latest version of the binary can be downloaded from https://github.com/rapidbus/rapidbus/releases

### Compilation from sources

Building is done using docker command - please install docker before running the build scripts. RapidBus DOES NOT RUN IN DOCKER BY DEFAULT, its merely using docker to build the binary so you dont have to install dependencies and build enviromnet on your system.

#### Building for Debian 11 (bullseye)

Compilation on Debian version before "bullseye" (Debian 11) is not supported as it does not include package https://packages.debian.org/bullseye/libpaho-mqtt-dev

Compilation is using PAHO MQTT C libraries directly from Debian repository and statically linking them.

```
git clone https://github.com/rapidbus/rapidbus.git
cd rapidbus/src
build-debian11.sh
# test newly-compiled binary:
./rapidbusd -c ./rapidbusd.conf.example
# install binary and template config file by copying
cp rapidbusd.conf.example /etc/rapidbusd.conf
cp rapidbusd /usr/local/bin
```

#### Building for RHEL 8

Compilation is using PAHO MQTT C libraries from https://github.com/eclipse/paho.mqtt.c repository and statically linking them.

```
git clone https://github.com/rapidbus/rapidbus.git
cd rapidbus/src
./build-rhel8.sh
# test newly-compiled binary:
./rapidbusd -c ./rapidbusd.conf.example
# install binary and template config file by copying
cp rapidbusd.conf.example /etc/rapidbusd.conf
cp rapidbusd /usr/local/bin
```

#### Building for RHEL 9

Compilation is using PAHO MQTT C libraries from https://github.com/eclipse/paho.mqtt.c repository and statically linking them.

```
git clone https://github.com/rapidbus/rapidbus.git
cd rapidbus/src
./build-rhel9.sh
# test newly-compiled binary:
./rapidbusd -c ./rapidbusd.conf.example
# install binary and template config file by copying
cp rapidbusd.conf.example /etc/rapidbusd.conf
cp rapidbusd /usr/local/bin
```

## Configuration

Each RapidBus daemon instance is configured using one isolated configuration file _rapidbusd.conf_ by default (please mind the "d" at the end of "rapidbusd" - as in daemon).

The default search path for the file is _/etc/rapidbusd.conf_ and can be changed with **-c** command-line options when starting **rapidbusd** daemon. Such as the following:

```
rapidbusd -c /etc/rapidbus/rapidbusd.conf
```

### Configuration of serial bus interface

Physical serial interfaces are a shared physical medium, but can contain multiple sensors with multiple baud and serial settings (for example using RS-485 signalling), therefore in RapidBus one physical interface can contain several "networks".
A network is a group of devices (sensors/nodes) sharing the same physical serial network settings - same serial interface, baud rate and serial settings (data bits, parity, stop bit).

There can be several virtual networks attached to one physical interface.

rapidbusd is designed to only manage one physical serial interface with one or multiple virtual networks. This is by design.

What must be defined in the rapidbusd configuration are the virtual networks that each individual sensor / node will fall into with its serial interface configuration:

```
v,vnet1,/dev/ttyUSB0,9600,8N1
v,vnet2,/dev/ttyUSB0,38400,8N1
```

Where in above example:
* v = mark identifying virtual network configuration in this line
* vnet1 = virtual network name
* /dev/ttyUSB1 = path to the serial bus device file (this must be the same for all virtual network interfaces!!)
* 38400 = baud rate (available options: 9600, 38400)
* 8N1 = 8 data bits, no parity, one stop bit (available options: 8N1)

Make sure that the user under which the rapidbusd will be running has permissions to access your desired serial port. In Debian 11 this means adding the user to the group "dialout".

### Configuration of clients (sensors)

Each RapidBus instance (rapidbusd "for RapidBus daemon") is attached to _one_ serial interface to schedule and read data from devices physically attached to that interface. The configuration file defines which MODBUS registries to read and then forward reponses to MQTT broker. RapidBus does not support MODBUS over IP - this is by design.

Multiple instances of rapidbusd are attached to multiple physical serial interfaces and have separate configuration files.

Which sensors RapidBus contacts and what data are read in what frequency (periods) are defined in this lines within configuration file.

Example of configuration file section for sensors (rapidbus-XXX.conf) (comment lines start with "#" and are ignored):

```
q,sensor1,net1,queryname1,1,3,5,2,f32,100
q,sensor1,net1,queryname2,1,3,9,2,s32,200
q,sensor1,net2,queryname2,1,3,9,2,u32,200
q,sensor2,net3,queryname1,2,3,9,1,s16,100
```

Where for first line:
* q = definition of "q"uery
* sensor1 = human name of sensor / node
* vnet1 = name of virtual network this devices is a part of
* queryname1 = human name of this query
* 1 = MODBUS ID
* 3 = request function
* 9 = start register
* 2 = number or words (word = 2 bytes)
* 100 = request every 100 ms (max. period = 65535 ms)

This example will make two requests to sensor we have named "sensor1" and one query to sensor2 periodically. Sensor1 has MODBUS ID "1" and MODBUS requests to this sensor will be:

```
# for sensor1/queryname1 (including CRC):
0x 01 03 00 05 00 02 d4 0a
# for sensor1/queryname2 (including CRC):
0x 01 03 00 09 00 02 14 09
```

Queryname1 and queryname2 is human-identifiable characteristic for this query which sill be used in MQTT published message.

### MQTT configuration

MQTT configuration is done in configuration file using one line in the following format:

```
m,tcp://broker.example.com:1883,rapidbus_client,pub_topic
```

* *m* = defines, that this line contains MQTT configuration
* *tcp://broker.example.com:1883* = MQTT broker protocol, address and port
* *rapidbus_client* = this MQTT client ID
* *pub_topic* = topic where messages will be published to

MQTT messages are always published with QOS=0 as RapidBus is intended mostly for fast-paced reads.

#### MQTT pub message format

```
{
    "node":"sensor1",
    "query":"queryname1",
    "value":123.456,
    "ts":1669728994372
}
```

* *node* is set to client / sensor name as defined in config file
* *query* is set to query name as defined in configuration file
* *value* is set to interpretet value of the MODBUS response
* *ts* is UNIX timestamp of when the query to the sensor was made in milliseconds

## Development

### Functional testing

```
# create virtual serial interface
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# in another terminal
cat < /dev/pts/2
```
