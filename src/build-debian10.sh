#!/bin/bash

set -e
set -x

VERSION=$(cat version.txt)
DISTRO="debian10"
DENGINE="docker"

$DENGINE build -f Dockerfile-Debian10 -t rapidbus/build-debian10 .
$DENGINE run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-debian10 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
rm -f rapidbusd-${VERSION}_${DISTRO}.tar.gz
tar czfv rapidbusd-${VERSION}_${DISTRO}.tar.gz rapidbusd rapidbusd.conf.example
