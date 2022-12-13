#!/bin/bash

set -e
set -x

VERSION=$(cat version.txt)
DISTRO="rhel8"
DENGINE="docker"

docker build -f Dockerfile-RHEL8 -t rapidbus/build-rhel8 .
docker run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-rhel8 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
rm rapidbusd-${VERSION}_${DISTRO}.tar.gz
tar czfv rapidbusd-${VERSION}_${DISTRO}.tar.gz rapidbusd rapidbusd.conf.example
