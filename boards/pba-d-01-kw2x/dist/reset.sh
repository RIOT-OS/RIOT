#!/bin/bash

openocd -f "$RIOTBASE/boards/pba-d-01-kw2x/dist/mkw22d512.cfg" \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
