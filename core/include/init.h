/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       Macros for initialisation of subsystem and drivers
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef INIT_H_
#define INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define INIT_ORDER_CORE             0
#define INIT_ORDER_POSTCORE         1
#define INIT_ORDER_PERIPHERAL       2
#define INIT_ORDER_BOARD            3
#define INIT_ORDER_SUBSYS           4
#define INIT_ORDER_SYSTEM           5
#define INIT_ORDER_DEVICE           6
#define INIT_ORDER_LIFTOFF          7

typedef int (*initcall_t)(void);

#define __system_initcall(fp, order) __initcall_macro(fp, order)

#define __initcall_macro(fp, order) static initcall_t __initcall_##fp##order \
                                    __attribute__((__used__)) \
                                    __attribute__((section(".preinit_array." #order))) \
                                    __attribute__((aligned(sizeof(void*)))) = fp

/**
 * @brief   Core Init
 * Setup cpu core, pll, clocks ...
 *
 * \note
 * RIOT can not output log (uart not initialized) at this point.
 *
 */
#define core_init(fp) __system_initcall(fp, INIT_ORDER_CORE)

/**
 * @brief   Postcore Init
 * Something urgent like setup the peripheral clocks and stdout
 *
 * \note
 * RIOT can not output log (uart not initialized) at this point,
 * but we can insert uart_stdio_init here.
 *
 */
#define postcore_init(fp) __system_initcall(fp, INIT_ORDER_POSTCORE)

/**
 * @brief   Peripheral Init
 * Peripheral initialization, e.g. uart, usb , spi.
 *
 * \note
 * RIOT should output log at this point,
 * because a lot of peripheral drivers use DEBUG.
 *
 */
#define peripheral_init(fp) __system_initcall(fp, INIT_ORDER_PERIPHERAL)

/**
 * @brief   Deck Init
 * Board depended initialization, e.g. leds.
 */
#define deck_init(fp) __system_initcall(fp, INIT_ORDER_BOARD)

/**
 * @brief   Subsystem Init
 * Subsystem initialization, e.g. ethernet over usb.
 */
#define subsys_init(fp) __system_initcall(fp, INIT_ORDER_SUBSYS)

/**
 * @brief   System Init
 * System initialization, e.g. network, usb stack, saul.
 */
#define system_init(fp) __system_initcall(fp, INIT_ORDER_SYSTEM)

/**
 * @brief   Device Init
 * Peripheral driver initialization, e.g. temperature sensor, propulsion system.
 */
#define device_init(fp) __system_initcall(fp, INIT_ORDER_DEVICE)

/**
 * @brief   Liftoff
 * Start services, e.g. coap-server.
 *
 * \note
 * All systems and devices should be initialized before this point.
 *
 */
#define liftoff_init(fp) __system_initcall(fp, INIT_ORDER_LIFTOFF)

#ifdef __cplusplus
}
#endif

#endif /* INIT_H_ */
/** @} */
