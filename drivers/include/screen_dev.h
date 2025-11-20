/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_screen_dev Screen device generic API
 * @ingroup     drivers_display
 * @brief       Define the generic API of a screen device
 *
 * The screen device API is a generic API built on top of display and touch
 * device APIs.
 *
 * Each display/touch device driver implementing this interface has to expose
 * a set of predefined functions and it has to register itself to the central
 * display/touch device registry. From here devices can be found, listed, and
 * accessed.
 *
 * The display and touch devices are linked to a screen by providing the
 * screen id (basically an index) they correspond to.
 *
 * @see drivers_disp_dev @see drivers_touch_dev
 *
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "disp_dev.h"

#ifdef MODULE_TOUCH_DEV
#include "touch_dev.h"
#endif

/**
 * @brief   Screen device descriptor
 */
typedef struct {
    disp_dev_t *display;            /**< Pointer to the display device */
#if MODULE_TOUCH_DEV || DOXYGEN
    touch_dev_t *touch;             /**< Pointer to the touch device */
#endif
} screen_dev_t;

#ifdef __cplusplus
}
#endif

/** @} */
