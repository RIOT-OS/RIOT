/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    net_telnet_stdio    STDIO via telnet
 * @ingroup     sys_stdio
 * @{
 *
 * @file
 * @brief       STDIO over Telnet implementation
 *
 * This file implements STDIO via a Telnet server with fallback UART output
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>

#include "board.h"
#include "kernel_defines.h"
#include "net/telnet.h"
#include "stdio_base.h"

STDIO_PROVIDER(STDIO_TELNET, NULL, telnet_server_disconnect, telnet_server_write)
