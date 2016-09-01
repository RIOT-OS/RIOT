/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Common header for at86rf2xx tests
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#include "pcap_dev.h"
#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

extern pcap_dev_t dev;

void recv(netdev2_t *dev);
int ifconfig(int argc, char **argv);
int txtsnd(int argc, char **argv);
void print_addr(uint8_t *addr, size_t addr_len);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H_ */
/** @} */
