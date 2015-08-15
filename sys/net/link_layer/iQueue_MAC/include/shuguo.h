/*
 * Copyright (C) 2015  Zhejiang University, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup MAC
 * @{
 * @brief   Head file of the iQueue-MAC protocol for RIOT OS test
 * @author  zhuo shuguo <zhuosg@zju.edu.cn>
 */

#ifndef _IQUEUE_MAC_H
#define _IQUEUE_MAC_H

#include "mac_driver.h"

extern const mac_driver_t iqueue_driver;

extern void iqueue_receive_packet(void *buf, unsigned int len,
                                  uint8_t rssi, uint8_t lqi,
                                  bool crc_ok);

extern void mac_rx_packet_process(void *buf,
                                  unsigned int len,
                                  uint8_t rssi,
                                  uint8_t lqi,
                                  bool crc_ok);

#endif /* _IQUEUE_MAC_H */
