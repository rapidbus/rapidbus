#!/bin/bash

# do not continue if nor root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root as this script uses docker to build the binaries"
  exit 1
fi

set -e
set -x

DISTRO=$1
VERSION=`cat version.txt`

docker build -f Dockerfile-$DISTRO -t rapidbus/build-$DISTRO .
docker run -it -v `pwd`:/opt/rapidbus --entrypoint /bin/sh rapidbus/build-$DISTRO -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
rm -f rapidbusd-$VERSION_$DISTRO.tar.gz
tar czfv rapidbusd-$VERSION_$DISTRO.tar.gz rapidbusd rapidbusd.conf.example