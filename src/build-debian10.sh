#!/bin/bash

# podman or docker
DENGINE="docker"

set -e

$DENGINE build -f Dockerfile-Debian10 -t rapidbus/build-debian10 .
$DENGINE run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-debian10 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
