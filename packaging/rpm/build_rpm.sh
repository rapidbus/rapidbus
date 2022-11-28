#!/bin/bash

set -e

VERSION=0.1.2

mkdir -p SOURCES
cd SOURCES
tar -cvf rapidbus-${VERSION}.tar.gz ../src/
cd ..
rpmbuild -v -bb --clean SPECS/rapidbus.spec
