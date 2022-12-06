FROM debian:11

RUN apt-get update && apt-get clean && rm -rf /var/lib/apt/lists/*
RUN apt-get install -y --no-install-recommends gcc make libpaho-mqtt-dev libpaho-mqtt1.3 libbsd-dev

COPY . /opt/rapidbus/
WORKDIR /opt/rapidbus/

RUN make
RUN ./rapidbusd
