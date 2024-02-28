/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotconf_slot   Helpers to manipulate RIOT configuration partitions
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       RIOT configuration partition management
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#ifndef RIOTCONF_SLOT_H
#define RIOTCONF_SLOT_H

#include <stddef.h>
#include "riotconf/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RIOTCONF_SLOT_NUMOF
/**
 * @brief   Number of configuration slots
 */
#define RIOTCONF_SLOT_NUMOF    2
#endif

/**
 * @brief   A user supplied callback to evaluate the suitability of a valid configuration slot
 */
typedef int (*riotconf_slot_compat_cb_t) (const riotconf_hdr_t *hdr);

/**
 * @brief   Get the configuration slot with the highest sequence number
 *
 * @param[in]   compatible  Callback to evaluate the suitability of a valid configuration slot
 *                          - for example to check for a required version -
 *
 * @return  The configuration slot with the highest sequence number
 */
riotconf_slot_t riotconf_slot_highest_seq(riotconf_slot_compat_cb_t compatible);

/**
 * @brief   Get an updatable configuration slot which is either invalid or has a lowest sequence number
 *          but is not the current slot
 *
 * @param[in]   current     The current configuration slot
 *
 * @return  The configuration slot of the slot to update
 */
riotconf_slot_t riotconf_slot_other(riotconf_slot_t current);

/**
 * @brief   The application can mark a valid slot with any encoded state
 *
 * The @ref RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED flag is reserved internally.
 * The @ref RIOTCONF_HDR_STATE_CONFIGURATION_VERIFIED may be set if the configuration
 * has been verified for correctness.
 *
 * @param[in]   slot    Configuration slot
 * @param[in]   state   State to encode
 *
 * @return  The state of the slot
 */
int riotconf_slot_set_state(riotconf_slot_t slot, uint32_t state);

/**
 * @brief   Read configuration data from a slot
 *
 * @param[in]   slot    Configuration slot
 * @param[in]   data    Buffer to read data into
 * @param[in]   offset  Offset in data to read from
 * @param[in]   size    Number of bytes to read
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_slot_read(riotconf_slot_t slot, void *data, size_t offset, size_t size);

/**
 * @brief   Open the storage device for reading configuration data
 *
 * @post The returned buffer @p buf contains the configuration header
 *       but it may not contain valid data yet.
 *
 * @param[in]   slot    Configuration slot
 * @param[out]  buf     Reurns a pointer to an internal sector buffer
                        to relieve the caller from allocating memory
 * @param[out]  size    Size of the returned buffer
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_slot_start_read(riotconf_slot_t slot, void **buf, size_t *size);

/**
 * @brief   Finish reading configuration data
 *
 * @param[in]   slot    Configuration slot
 */
void riotconf_slot_finish_read(riotconf_slot_t slot);

/**
 * @brief   Start writing configuration data
 *
 * The first sector containing the header is saved before erase.
 *
 * @note  If the output header has a valid magic number, you may assume that also version and sequence number are valid.
 *
 * @warning The configuration on the device is invalidated to not have valid but partially written data
 *
 * @param[in]   slot    Configuration slot
 * @param[out]  hdr     If not NUll, it will contain the header of the current slot which was erased
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_slot_start_write(riotconf_slot_t slot, riotconf_hdr_t *hdr);

/**
 * @brief   Write configuration data to a slot
 *
 * @param[in]   slot    Configuration slot
 * @param[in]   data    Buffer to write data from
 * @param[in]   offset  Offset in data to write to
 * @param[in]   size    Number of bytes to write
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_slot_write(riotconf_slot_t slot, const void *data, size_t offset, size_t size);

/**
 * @brief   Finish writing configuration data
 *
 * This function writes a new header with provided values and calculates the checksum.
 *
 * @param[in]   slot    Configuration slot
 * @param[in]   seq     Sequence number to write to the header
 * @param[in]   version Version number to write to the header
 * @param[in]   size    Size of the configuration data
 */
void riotconf_slot_finish_write(riotconf_slot_t slot, uint32_t seq, uint32_t version, size_t size);

/**
 * @brief   Validate a configuration slot, that means the header contains the magic number
 *          and the checksum is correct
 *
 * @param[in]   slot    Configuration slot
 * @param[out]  hdr     Header that has been written to the slot
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_slot_validate(riotconf_slot_t slot, riotconf_hdr_t *hdr);

/**
 * @brief   Invalidate a configuration slot
 *
 * @param[in]   slot    Configuration slot
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_slot_invalidate(riotconf_slot_t slot);

#ifdef __cplusplus
}
#endif
#endif /* RIOTCONF_SLOT_H */
