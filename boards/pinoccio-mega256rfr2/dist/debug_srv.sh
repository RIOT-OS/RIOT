#!/usr/bin/env bash

sleep 2
avarice $1

# avarice exits with 1 if the connection is released, therefore we always exit with 0
exit 0
