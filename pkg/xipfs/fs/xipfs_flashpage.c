/*
 * SPDX-FileCopyrightText: 2024 Université de Lille
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
 * @ingroup     sys_fs_xipfs
 * @{
 *
 * @file
 * @brief       xipfs integration with flashpage
 *
 * @author      Damien Amara <damien.amara@univ-lille.fr>
 *
 * @}
 */

#include "periph/flashpage.h"

void *xipfs_nvm_addr(unsigned page)
{
    return flashpage_addr(page);
}

void xipfs_nvm_erase(unsigned page)
{
    flashpage_erase(page);
}

unsigned xipfs_nvm_page(const void *addr)
{
    return flashpage_page(addr);
}

void xipfs_nvm_write(void *target_addr, const void *data, size_t len)
{
    flashpage_write(target_addr, data, len);
}
