/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  drivers_pcap_dev
 * @brief   File sink/drain for PCAP device on @ref native_cpu
 * @{
 *
 * @file
 * @brief   File sink/drain definitions for PCAP device on @ref native_cpu
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef PCAP_DEV_SD_FILE_H_
#define PCAP_DEV_SD_FILE_H_

#include <stdbool.h>
#include <sys/stat.h>

#include "pcap_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   State of file sink/drain
 */
typedef struct {
    const char *sink_fname;     /**< file name of sink */
    /**
     * @brief   file name of drain
     *
     * May be NULL if no drain exists.
     * May also be set to NULL by pcap_dev_sd_t::init() if no file of name
     * pcap_dev_sd_file_state_t::drain_fname exists.
     */
    char *drain_fname;
    size_t drain_offset;        /**< current offset in drain file */
    bool drain_swapped;         /**< drain's endianness differs from host's endianness */
    mode_t sink_mode;           /**< permissions for the sink file */
} pcap_dev_sd_file_state_t;

/**
 * @brief   sink/drain descriptor for files
 */
extern const pcap_dev_sd_t pcap_dev_sd_file;

#ifdef __cplusplus
}
#endif

#endif /* PCAP_DEV_SD_FILE_H_ */
/** @} */
