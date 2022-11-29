## Bridge to forward data from physical serial to MQTT

RapidBus is a bridge to collect data from MODBUS (RTU) enabled sensors and rapidly forward them to MQTT broker for further analysis.

RapidBus is used in OT/IT environments where its necessary to bridge the physical sensors with IT infrastructure. RapidBus is usually configured to read data in high frequency from multiple sensors on one physical serial (RS-485 for example) interface using RTU MODBUS protocol. Running multiple instances of RapidBus daemon its possible to mamage musltiple physical serial interfaces on one edge system in Industry 4.0 architecture.

## Configuration of serial bus interface

Make sure that the user under which the rapidbusd will be running has permissions to access your desired serial port. In Debian 11 this means adding the user to the group "dialout".

```
s,/dev/ttyUSB1,38400,8N1
```

Where:
* s = this line contained serial configuration options
* /dev/ttyUSB1 = path to the bus device file
* 38400 = baud rate (available options: 38400)
* 8N1 = 8 data bits, no parity, one stop bit (available options: 8N1)

## Configuration of clients (sensors)

Each RapidBus instance (rapidbusd "for RapidBus daemon") is attached to _one_ serial interface to schedule and read data from devices physically attached to that interface. The configuration file defines which MODBUS registries to read and then forward reponses to MQTT broker. RapidBus does not support MODBUS over IP as we believe there are multiple other solutions that are more suitable for direct IP communication with IP enabled sensors and devices.

Multiple instances of rapidbusd are attached to multiple physical serial interfaces and have separate configuration files.

Which sensors RapidBus contacts and what data are read in what frequency (periods) are defined in this configuration file.

Example of configuration file (rapidbus-XXX.conf) (comment lines start with "#" and are ignored):

```
q,sensor1,queryname1,1,3,5,2,100
q,sensor1,queryname2,1,3,9,2,200
q,sensor2,queryname1,2,3,9,2,100
```

Where for first line:
* q = definition of "q"uery
* sensor1 = human name of sensor / node
* queryname1 = human name of sensor / node
* 1 = MODBUS ID
* 3 = request function
* 9 = start register
* 2 = number or words (word = 2 bytes)
* 100 = request every 100 ms (max. period = 65535 ms) 

This example will make two requests to sensor we have named "sensor1" and one query to sensor2 periodically. Sensor1 has MODBUS ID "1" and MODBUS requests to this sensor will be:

```
# for queryname1 (including CRC):
0x 01 03 00 05 00 02 d4 0a
# for queryname2 (including CRC):
0x 01 03 00 09 00 02 14 09
```

Queryname1 and queryname2 is human-identifiable characteristic for this query which sill be used in MQTT published message.

## MQTT configuration

MQTT configuration is done in configuration file using one line in the following format:

```
m,tcp://broker.example.com:1883,rapidbus_client,pub_topic
```

* *m* = defines, that this line contains MQTT configuration
* *tcp://broker.example.com:1883* = MQTT broker protocol, address and port
* *rapidbus_client* = this MQTT client ID
* *pub_topic* = topic where messages will be published to

MQTT messages are always published with QOS=0 as RapidBus is intended mostly for fast-paced reads.

### MQTT pub message format

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
