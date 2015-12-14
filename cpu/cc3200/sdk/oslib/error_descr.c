/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Attilio Dona'
 *
 */

#include "throw.h"

const char* sl_err_descr[] = {
        [NO_ERROR] = "no error",
        [GEN_ERROR] = "simplelink internal error",
        [SIMPLELINK_START_ERROR] = "unable to start simplelink task",
        [SMARTCONFIG_ERROR] = "unable to setup smart config",
        [WLAN_SET_POLICY_ERROR] = "wlan set policy failed",
        [SOCKET_OPEN_FAIL] = "socket open failed",
        [SOCKET_CONNECT_FAIL] = "socket connect failed"
};



