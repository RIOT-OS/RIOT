/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file is subject to the terms and conditions of the LGPLv2.
See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#ifndef CPUCONF_H_
#define CPUCONF_H_

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (256)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT	(256)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE 96
#define MSP430_ISR_STACK_SIZE 256

#define RX_BUF_SIZE (3)
#define TRANSCEIVER_BUFFER_SIZE (3)
/** @} */

#endif /* CPUCONF_H_ */
