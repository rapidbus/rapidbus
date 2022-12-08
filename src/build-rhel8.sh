#!/bin/bash

set -e

docker build -f Dockerfile-RHEL8 -t rapidbus/build-rhel8 .
docker run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-rhel8 -c "cd /opt/rapidbus/; make all"
