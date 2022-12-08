#!/bin/bash

set -e

docker build -f Dockerfile-Debian11 -t rapidbus/build-debian11 .
docker run -it -v `pwd`:/opt/rapidbus --entrypoint /bin/bash rapidbus/build-debian11 -c "cd /opt/rapidbus/; make all"
