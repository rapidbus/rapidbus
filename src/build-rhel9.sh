#!/bin/bash

set -e

docker build -f Dockerfile-RHEL9 -t rapidbus/build-rhel9 .
docker run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-rhel9 -c "cd /opt/rapidbus/; make all; ./rapidbusd -h"
