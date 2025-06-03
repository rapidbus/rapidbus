# rapidbus

[![Coverity Scan Build Status](https://scan.coverity.com/projects/27066/badge.svg)](https://scan.coverity.com/projects/rapidbus-rapidbus)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/a74f50557c9548d0afdb4dd5df780d11)](https://app.codacy.com/gh/rapidbus/rapidbus/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![SonarCloud Bugs](https://sonarcloud.io/api/project_badges/measure?project=rapidbus_rapidbus&metric=bugs)](https://sonarcloud.io/summary/new_code?id=rapidbus_rapidbus)

rapidbus is a bridging application between Operational Technologies and Information Technologies (OT/IT) connecting serial-enabled sensors to MQTT broker. Essentially forwarding metering data from serial interface over MODBUS RTU to MQTT broker.

This program is written in C (standard: gnu99 (ISO C 1999 with GNU extensions)) and targeted to be used on Linux platforms on the edge of Industry 4.0 networks. For detailed documentation see here: https://www.rapidbus.org

* Its designed to be very stable
* Written in modern C (not C++!!)
* Adheres to C standards
* External libraries are statically linked so no dependencies should be necessary
* Simple text-based (CVS) configuration script - can even be managed through Excel and commited into git
* One binary file, no external files
* Designed to be fail-proof - for example if network crashes the connections will be re-created and software will not stop
* Designed to be used with minimal administration overhead
