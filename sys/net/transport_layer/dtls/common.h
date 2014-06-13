/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup dtls
 * @{
 * @file
 * @brief   common functions for DTLS
 * @author  Jonatan Zint <j.zint@fu-berlin.com>
 * @author  Nico von Geyso <nico.geyso@fu-berlin.com>
 */

#ifndef DTLS_PROTOCOLS_RECORD_H_
#define DTLS_PROTOCOLS_RECORD_H_

#define ASSIGN_UINT32_TO_UINT24(src, dest)  \
  do { \
    uint8_t *ptr = (uint8_t*) &(dest); \
    *ptr = src & 0xFF; ++ptr; \
    *ptr = (src >>  8) & 0xFF; ++ptr;\
    *ptr = (src >> 16) & 0xFF; ++ptr;\
  } while (0)

#define ASSIGN_UINT64_TO_UINT48(src, dest)  \
  do { \
    uint16_t *ptr = (uint16_t*) &(dest); \
    *ptr = src & 0xFFFF; ++ptr; \
    *ptr = (src >> 16) & 0xFFFF; ++ptr;\
    *ptr = (src >> 32) & 0xFFFF; ++ptr;\
  } while (0)

#endif
/**
 * @}
 */
