FROM debian:11

RUN apt-get update
RUN apt-get install -y gcc make libpaho-mqtt-dev libpaho-mqtt1.3 libbsd-dev

COPY . /opt/rapidbus/
WORKDIR /opt/rapidbus/

RUN make
RUN ./rapidbusd
