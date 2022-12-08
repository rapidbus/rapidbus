#!/bin/bash

set -e

docker build -f Dockerfile-Debian10 -t rapidbus/build-debian10 .
docker run -it -v $(pwd):/opt/rapidbus --entrypoint /bin/bash rapidbus/build-debian10 -c "cd /opt/rapidbus/; make all"
