#!/bin/bash

# podman or docker
DENGINE="podman"

set -e

$DENGINE build -f Dockerfile-Debian11 -t rapidbus/build-debian11 .
$DENGINE run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-debian11 -c "cd /opt/rapidbus/; make all"
