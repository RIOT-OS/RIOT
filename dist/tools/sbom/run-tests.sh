#!/bin/bash

SCRIPT_DIR=$(dirname $(readlink -f "$0"))

exec python -m unittest  discover -s ${SCRIPT_DIR}/src -p '*.py'
