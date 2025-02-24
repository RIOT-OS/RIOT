#!/bin/sh
#
# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

case ${DEBUG_ADAPTER_ID} in
    # check if ${DEBUG_ADAPTER_ID} is empty or contains a number
    ''|*[!0-9]*)
        echo "Please provide native instance's PID using DEBUG_ADAPTER_ID" >&2
        exit 1
        ;;
    *)  ;;
esac

kill -USR1 "${DEBUG_ADAPTER_ID}"
