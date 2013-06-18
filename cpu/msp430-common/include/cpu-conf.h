/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file subject to the terms and conditions of the GNU Lesser General Public
License. See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#ifndef CPUCONF_H_
#define CPUCONF_H_

/**
 * @name Kernel configuration
 * @{
 */
#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT	1024
#endif

#define KERNEL_CONF_STACKSIZE_IDLE 64
#define MSP430_ISR_STACK_SIZE 256

#define RX_BUF_SIZE (3)
#define TRANSCEIVER_BUFFER_SIZE (3)
/** @} */

#endif /* CPUCONF_H_ */
