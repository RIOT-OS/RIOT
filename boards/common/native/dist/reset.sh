#!/bin/sh

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

case ${DEBUG_ADAPTER_ID} in
    # check if ${DEBUG_ADAPTER_ID} is empty or contains a number
    ''|*[!0-9]*)
        echo "Please provide native instance's PID using DEBUG_ADAPTER_ID" >&2
        exit 1
        ;;
    *)  ;;
esac

kill -USR1 "${DEBUG_ADAPTER_ID}"
