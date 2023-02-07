# rapidbus

[![Coverity Scan Build Status](https://scan.coverity.com/projects/27066/badge.svg)](https://scan.coverity.com/projects/rapidbus-rapidbus)
[![CodeQL](https://github.com/rapidbus/rapidbus/actions/workflows/codeql.yml/badge.svg)](https://github.com/rapidbus/rapidbus/actions/workflows/codeql.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/8f60348054ba49e9b39f5a71de5109c2)](https://www.codacy.com/gh/rapidbus/rapidbus/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=rapidbus/rapidbus&amp;utm_campaign=Badge_Grade)
[![SonarCloud Bugs](https://sonarcloud.io/api/project_badges/measure?project=rapidbus_rapidbus&metric=bugs)](https://sonarcloud.io/summary/new_code?id=rapidbus_rapidbus)

rapidbus is a bridging application between Operational Technologies and Information Technologies (OT/IT) connecting serial-enabled sensors to MQTT broker. Essentially forwarding metering data from serial interface over MODBUS RTU to MQTT broker.

This program is written in C (standard: gnu99 (ISO C 1999 with GNU extensions)) and targeted to be used on Linux platforms on the edge of Industry 4.0 networks. For detailed documentation see here: https://www.rapidbus.org

  *  Its designed to be very stable
  *  Written in modern C (not C++!!)
  *  Adheres to C standards
  *  External libraries are statically linked so no dependencies should be necessary
  *  Simple text-based (CVS) configuration script - can even be managed through Excel and commited into git
  *  One binary file, no external files
  *  Designed to be fail-proof - for example if network crashes the connections will be re-created and software will not stop
  *  Designed to be used with minimal administration overhead
