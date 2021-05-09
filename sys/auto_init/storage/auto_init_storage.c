/*
 * Copyright 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup sys_auto_init_storage Auto-initialization of storage devices
 * @ingroup  sys_auto_init
 *
 * @{
 * @file
 * @brief   Implementation of the auto initialization of storage devices
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "kernel_defines.h"
#include "log.h"

void auto_init_storage(void)
{
    if (IS_USED(MODULE_AUTO_INIT_STORAGE_AT24CXXX)) {
        LOG_DEBUG("Auto init at24cxxx.\n");
        extern void auto_init_at24cxxx(void);
        auto_init_at24cxxx();
    }
}
