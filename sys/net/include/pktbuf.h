/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pktbuf   Packet buffer
 * @ingroup     net
 * @brief       A global network packet buffer.
 * @{
 *
 * @file    pktbuf.h
 * @brief   Interface definition for the global network buffer. Network devices
 *          and layers can allocate space for packets here.
 *
 * @note    **WARNING!!** Do not store data structures that are not packed
 *          (defined with `__attribute__((packed))`) or enforce alignment in
 *          in any way in here. On some RISC architectures this *will* lead to
 *          alignment problems and can potentially result in segmentation/hard
 *          faults and other unexpected behaviour.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __PKTBUF_H_
#define __PKTBUF_H_

#include <stdlib.h>

#include "cpu-conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PKTBUF_SIZE
/**
 * @brief   Maximum size of the packet buffer.
 *
 * @details The rational here is to have at least space for 4 full-MTU IPv6
 *          packages (2 incoming, 2 outgoing; 2 * 2 * 1280 B = 5 KiB) +
 *          Meta-Data (roughly estimated to 1 KiB; might be smaller)
 */
#define PKTBUF_SIZE  (6144)
#endif  /* PKTBUF_SIZE */

/**
 * @brief   Allocates new packet data in the packet buffer. This also marks the
 *          allocated data as processed.
 *
 * @see @ref pktbuf_hold()
 *
 * @param[in] size  The length of the packet you want to allocate
 *
 * @return  Pointer to the start of the data in the packet buffer, on success.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 */
void *pktbuf_alloc(size_t size);

/**
 * @brief   Reallocates new space in the packet buffer, without changing the
 *          content.
 *
 * @details If enough memory is available behind it or *size* is smaller than
 *          the original size the packet will not be moved. Otherwise, it will
 *          be moved. If no space is available nothing happens.
 *
 * @param[in] pkt   Old position of the packet in the packet buffer
 * @param[in] size  New amount of data you want to allocate
 *
 * @return  Pointer to the (maybe new) position of the packet in the packet buffer,
 *          on success.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 *          The packet will remain at *ptr*.
 */
void *pktbuf_realloc(const void *pkt, size_t size);

/**
 * @brief   Allocates and copies new packet data into the packet buffer.
 *          This also marks the allocated data as processed for the current
 *          thread.
 *
 * @see @ref pktbuf_hold()
 *
 * @param[in] data  Data you want to copy into the new packet.
 * @param[in] size  The length of the packet you want to allocate
 *
 * @return  Pointer to the start of the data in the packet buffer, on success.
 * @return  NULL, if no space is left in the packet buffer.
 */
void *pktbuf_insert(const void *data, size_t size);

/**
 * @brief   Copies packet data into the packet buffer, safely.
 *
 * @details Use this instead of memcpy, since it is thread-safe and checks if
 *          *pkt* is
 *
 *          -# in the buffer at all
 *          -# its *size* is smaller or equal to the data allocated at *pkt*
 *
 *          If the *pkt* is not in the buffer the data is just copied as
 *          memcpy would do.
 *
 * @param[in,out] pkt   The packet you want to set the data for.
 * @param[in] data      The data you want to copy into the packet.
 * @param[in] data_len  The length of the data you want to copy.
 *
 * @return  *data_len*, on success.
 * @return  -EFAULT, if *data* is NULL and DEVELHELP is defined.
 * @return  -EINVAL, if *pkt* is NULL and DEVELHELP is defined.
 * @return  -ENOBUFS, if *data_len* was greater than the packet size of *pkt*.
 */
int pktbuf_copy(void *pkt, const void *data, size_t data_len);

/**
 * @brief   Marks the data as being processed.
 *
 * @details Internally this increments just a counter on the data.
 *          @ref pktbuf_release() decrements it. If the counter is <=0 the
 *          reserved data block in the buffer will be made available again.
 *
 * @param[in] pkt   The packet you want mark as being processed.
 */
void pktbuf_hold(const void *pkt);

/**
 * @brief   Marks the data as not being processed.
 *
 * @param[in] pkt   The packet you want mark as not being processed anymore.
 *
 * @details Internally this decrements just a counter on the data.
 *          @ref pktbuf_hold() increments and any allocation
 *          operation initializes it. If the counter is <=0 the reserved data
 *          block in the buffer will be made available again.
 */
void pktbuf_release(const void *pkt);

/**
 * @brief   Prints current packet buffer to stdout if DEVELHELP is defined.
 */
#ifdef DEVELHELP
void pktbuf_print(void);
#else
#define pktbuf_print()  ;
#endif

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Counts the number of allocated bytes
 *
 * @return  Number of allocated bytes
 */
size_t pktbuf_bytes_allocated(void);

/**
 * @brief   Counts the number of allocated packets
 *
 * @return  Number of allocated packets
 */
size_t pktbuf_packets_allocated(void);

/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  1, if packet buffer is empty
 * @return  0, if packet buffer is not empty
 */
int pktbuf_is_empty(void);

/**
 * @brief   Checks if a given pointer is stored in the packet buffer
 *
 * @param[in] pkt   Pointer to be checked
 *
 * @return  1, if *pkt* is in packet buffer
 * @return  0, otherwise
 */
int pktbuf_contains(const void *pkt);

/**
 * @brief   Sets the whole packet buffer to 0
 */
void pktbuf_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PKTBUF_H_ */
/** @} */
