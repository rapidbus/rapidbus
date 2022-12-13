#!/bin/bash

set -e
set -x

VERSION=$(cat version.txt)
DISTRO="debian11"
DENGINE="docker"

$DENGINE build -f Dockerfile-Debian11 -t rapidbus/build-debian11 .
$DENGINE run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-debian11 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
tar czfv rapidbusd-${VERSION}_${DISTRO}.tar.gz rapidbusd rapidbusd.conf.example
