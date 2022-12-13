#!/bin/bash

set -e
set -x

VERSION=$(cat version.txt)
DISTRO="alpine3"
DENGINE="docker"

$DENGINE build -f Dockerfile-Alpine3 -t rapidbus/build-alpine3 .
$DENGINE run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/sh rapidbus/build-alpine3 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
rm -f rapidbusd-${VERSION}_${DISTRO}.tar.gz
tar czfv rapidbusd-${VERSION}_${DISTRO}.tar.gz rapidbusd rapidbusd.conf.example
