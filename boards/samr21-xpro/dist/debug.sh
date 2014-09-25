#!/bin/bash

openocd -f "board/atmel_samr21_xplained_pro.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "init" \
    -c "targets" \
    -c "reset halt"
