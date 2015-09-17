/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
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

#define INIT_ORDER_CORE         0
#define INIT_ORDER_DRIVER       1
#define INIT_ORDER_SUBMOD       2
#define INIT_ORDER_MODULE       3

typedef int (*initcall_t)(void);

#define __system_initcall(name, fp, order) static initcall_t __initcall_##fp##order \
                                           __attribute__((__used__)) \
                                           __attribute__((section(".preinit_array." #order "." name))) \
                                           __attribute__((aligned(sizeof(void*)))) = fp

#define core_init(fp) __system_initcall("core", fp, 0)

#define driver_init(fp) __system_initcall("driver", fp, 1)

#define submod_init(fp) __system_initcall("submod", fp, 2)

#define module_init(fp) __system_initcall("module", fp, 3)


#ifdef __cplusplus
}
#endif

#endif /* INIT_H_ */
/** @} */
