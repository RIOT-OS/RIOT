/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPUCONF_H_
#define CPUCONF_H_

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (256)
#define KERNEL_CONF_STACKSIZE_PRINTF_FLOAT (KERNEL_CONF_STACKSIZE_PRINTF)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (256)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (96)
#define MSP430_ISR_STACK_SIZE           (256)

#define RX_BUF_SIZE (3)
#define TRANSCEIVER_BUFFER_SIZE (3)

#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (32)
#endif
/** @} */

#endif /* CPUCONF_H_ */
