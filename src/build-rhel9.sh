#!/bin/bash

set -e
set -x

VERSION=$(cat version.txt)
DISTRO="rhel9"
DENGINE="docker"

docker build -f Dockerfile-RHEL9 -t rapidbus/build-rhel9 .
docker run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-rhel9 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
tar czfv rapidbusd-${VERSION}_${DISTRO}.tar.gz rapidbusd rapidbusd.conf.example
