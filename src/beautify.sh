#!/bin/bash

set -e

#python -m black -l 120 *.py
#python -m pylint *.py

scan-build-11 --status-bugs make
