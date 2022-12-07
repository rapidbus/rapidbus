# RapidBus

[![Coverity Scan Build Status](https://scan.coverity.com/projects/27066/badge.svg)](https://scan.coverity.com/projects/rapidbus-rapidbus)
[![CodeQL](https://github.com/rapidbus/rapidbus/actions/workflows/codeql.yml/badge.svg)](https://github.com/rapidbus/rapidbus/actions/workflows/codeql.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/8f60348054ba49e9b39f5a71de5109c2)](https://www.codacy.com/gh/rapidbus/rapidbus/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=rapidbus/rapidbus&amp;utm_campaign=Badge_Grade)

RapidBus is a bridging application between Operational Technologies and Information Technologies (OT/IT) connecting serial-enabled sensors to MQTT broker. Essentially forwarding metering data from serial interface over MODBUS RTU to MQTT broker.

This program is written in C (compiler: clang 13; standard: gnu99 (ISO C 1999 with GNU extensions)) and targeted to be used on Linux platforms on the edge of Industry 4.0 networks. For detailed documentation see here: https://www.rapidbus.org

## Preparing development environment

```
# create virtual serial interface
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# in another terminal
cat < /dev/pts/2
```

### RHEL

#### RHEL 9

You might need to install EPEL repository to get all necessary packages.

```
yum install paho-c cppcheck rpm-build clang libbsd-devel
make
sudo make install
```
