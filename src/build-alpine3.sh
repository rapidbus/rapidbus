#!/bin/bash

# podman or docker
DENGINE="docker"

set -e

$DENGINE build -f Dockerfile-Alpine3 -t rapidbus/build-alpine3 .
$DENGINE run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/sh rapidbus/build-alpine3 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
tar -czfv rapidbusd.tar.gz rapidbusd rapidbusd.conf.example
