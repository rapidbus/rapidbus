#!/bin/bash

set -e

read -r VERSION < ../../src/version.txt

mkdir -p SOURCES
cd SOURCES
tar -cvf rapidbus-${VERSION}.tar.gz ../../../src/
cd ..
rpmbuild -v -bb --clean SPECS/rapidbus.spec
