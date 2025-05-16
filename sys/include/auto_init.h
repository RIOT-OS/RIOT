/*
 * Copyright (C) 2010,2015 Freie Universität Berlin
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_auto_init Auto-initialization
 * @ingroup     sys
 * @brief       Auto initialize modules
 *
 * This feature is automatically enabled and can be disabled in any application
 * by disabling the `auto_init` module in the application's `Makefile`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * DISABLE_MODULE += auto_init
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * `auto_init` initializes any included module that provides
 * auto-initialization capabilities.
 * This concerns first any other included module that does not require a
 * parameter in its init function, i.e. if the init prototype looks like this:
 * `void MODULE_init(void)`. Most timer modules or simple drivers can be
 * initialized by `auto_init`.
 * The modules will be initialized in the context of the main thread right
 * before the main function gets called. Be aware that most modules expect to
 * be initialized only once, so do not call a module's init function when using
 * `auto_init` unless you know what you're doing.
 *
 * More complex device [drivers](@ref drivers), for example
 * [SAUL](@ref drivers_saul) drivers or
 * [network device drivers](@ref drivers_netdev), can also be initialized
 * automatically using the `auto_init` module.
 * To do so, each driver implementation must provide default initialization
 * parameters in the `DRIVER_params.h` file.
 * These parameters can be overridden from the application code in several ways
 * (examples with the @ref drivers_bmp180 oversampling parameter
 * `BMP180_PARAM_OVERSAMPLING`):
 *
 *  - by passing them via the `CFLAGS` variable on the build command line:
 *
 * ```
 * CFLAGS=-DBMP180_PARAM_OVERSAMPLING=1 USEMODULE=bmp180 make BOARD=arduino-zero -C examples/basic/default
 * ```
 *
 * - by setting the `CFLAGS` variable in the application `Makefile`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DBMP180_PARAM_OVERSAMPLING=1
 * ~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - by copying the `bmp180_params.h` header to the application directory and
 * editing it there with the desired values. This file is be included first
 * and thus the one from the driver implementation is skipped.
 *
 * From low-level CPU peripheral, the default initialization parameters are
 * defined in each board configuration that provides them.
 */

/**
 * @defgroup    sys_auto_init_saul SAUL drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of SAUL drivers
 *
 * @see @ref drivers_saul, @ref sys_auto_init
 */

/**
 * @defgroup    sys_auto_init_gnrc_netif GNRC netif drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of
 * [network device drivers](@ref drivers_netdev) via GNRC
 *
 * @see @ref net_gnrc_netif, @ref sys_auto_init
 */

/**
 * @defgroup    sys_auto_init_lwip_netif lwIP netif drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of network device drivers for lwIP
 *
 * @see @ref pkg_lwip, @ref sys_auto_init
 */

/**
 * @defgroup    sys_auto_init_multimedia Multimedia driver auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of Multimedia drivers
 *
 * @see @ref drivers_multimedia, @ref sys_auto_init
 */

/**
 * @{
 *
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes all high level modules that do not require parameters for
 *        initialization or uses default values.
 *
 *        This function gets called - if not explicitly disabled - by @ref
 *        kernel_init right before jumping into @e main.
 */
void auto_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
