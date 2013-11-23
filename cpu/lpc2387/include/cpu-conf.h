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
 * @ingroup		conf
 * @ingroup		lpc2387
 *
 * @{
 */

/**
 * @file
 * @brief		LPC2387 CPUconfiguration
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		baar
 * @version     $Revision$
 *
 * @note		$Id$
 */

#define FEUERWARE_CONF_CPU_NAME			"NXP LPC2387"

/**
 * @name CPU capabilities
 * @{
 */
#define FEUERWARE_CPU_LPC2387					1
#define FEUERWARE_CONF_CORE_SUPPORTS_TIME		1
/** @} */

/**
 * @name Stdlib configuration
 * @{
 */
#define __FOPEN_MAX__		4
#define __FILENAME_MAX__	12
/** @} */

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (4096)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT	(512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE		(160)
/** @} */

/**
 * @name Compiler specifics
 * @{
 */
#define CC_CONF_INLINE					inline
#define CC_CONF_USED					__attribute__((used))
#define CC_CONF_NONNULL(...)			__attribute__((nonnull(__VA_ARGS__)))
#define CC_CONF_WARN_UNUSED_RESULT		__attribute__((warn_unused_result))
/** @} */

#define TRANSCEIVER_BUFFER_SIZE (10)
#define RX_BUF_SIZE  (10)

/** @} */
#endif /* CPUCONF_H_ */
