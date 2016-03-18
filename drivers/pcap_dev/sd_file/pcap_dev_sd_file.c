/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <fcntl.h>

#include "byteorder.h"
#include "native_internal.h"
#include "pcap_dev.h"

#include "pcap_dev/sd_file.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _sd_file_init(pcap_dev_t *dev, const pcap_hdr_t *hdr);
static int _sd_file_output(pcap_dev_t *dev, const void *data, size_t data_len);
static int _sd_file_input(pcap_dev_t *dev, pcaprec_hdr_t *data, size_t max_data,
                          timex_t *next);

const pcap_dev_sd_t pcap_dev_sd_file = {
    .init = _sd_file_init,
    .output = _sd_file_output,
    .input = _sd_file_input,
};

static int _sd_file_init(pcap_dev_t *dev, const pcap_hdr_t *sink_hdr)
{
    pcap_hdr_t drain_hdr;
    pcap_dev_sd_file_state_t *state = dev->sd_state;
    int fd, res;

    if ((fd = real_open(state->sink_fname, O_WRONLY | O_CREAT | O_TRUNC,
                        state->sink_mode)) < 0) {
        DEBUG("_sd_file_init: Unable to open sink \"%s\"\n", state->sink_fname);
        return -errno;
    }
    if ((res = real_write(fd, sink_hdr, sizeof(pcap_hdr_t))) < 0) {
        DEBUG("_sd_file_init: Unable to write to sink \"%s\"\n", state->sink_fname);
        real_close(fd);
        return -errno;
    }
    real_close(fd);
    if ((state->drain_fname != NULL) &&
        (fd = real_open(state->drain_fname, O_RDONLY)) < 0) {
        if (errno != ENOENT) {
            DEBUG("_sd_file_init: Unable to open drain \"%s\"\n", state->drain_fname);
            return -errno;
        }
        DEBUG("_sd_file_init: drain \"%s\" does not exist\n", state->drain_fname);
        state->drain_offset = 0;
        state->drain_fname = NULL;
    }
    else if ((state->drain_fname != NULL)) {
        if ((res = real_read(fd, &drain_hdr, sizeof(pcap_hdr_t))) < 0) {
            DEBUG("_sd_file_init: Unable to read from drain \"%s\"\n", state->drain_fname);
            real_close(fd);
            return -errno;
        }
        if (drain_hdr.magic_number == byteorder_swapl(PCAP_MAGIC_NUMBER)) {
            DEBUG("_sd_file_init: Drain \"%s\" in swapped host byte order\n",
                  state->drain_fname);
            /* TODO: add support for swapped PCAP */
            real_close(fd);
            return -EINVAL;
        }
        else {
            DEBUG("_sd_file_init: Drain \"%s\" in host byte order\n",
                  state->drain_fname);
            state->drain_swapped = false;
        }
        if ((drain_hdr.magic_number != PCAP_MAGIC_NUMBER) ||
            (drain_hdr.version_major != PCAP_VERSION_MAJOR) ||
            (drain_hdr.version_minor != PCAP_VERSION_MINOR) ||
            (drain_hdr.snaplen > 0xffff) ||
            (drain_hdr.net != dev->net)) {
            DEBUG("_sd_file_init: A field of the drain \"%s\"'s global header was\n",
                  state->drain_fname);
            DEBUG("               of invalid value:\n");
            DEBUG("               * magic_number: 0x%08" PRIx32 " (must be 0x%08" PRIx32 ")\n",
                  drain_hdr.magic_number, PCAP_MAGIC_NUMBER);
            DEBUG("               * version_major: %u (must be %u)\n",
                  (unsigned)drain_hdr.version_major, (unsigned)PCAP_VERSION_MAJOR);
            DEBUG("               * version_minor: %u (must be %u)\n",
                  (unsigned)drain_hdr.version_minor, (unsigned)PCAP_VERSION_MINOR);
            DEBUG("               * snaplen: %" PRIu32 " (must be <= %" PRIu32 ")\n",
                  drain_hdr.snaplen, 0xffffU);
            DEBUG("               * net: %" PRIu32 " (must be <= %" PRIu32 ")\n",
                  drain_hdr.net, dev->net);
            real_close(fd);
            return -EINVAL;
        }
        state->drain_offset = res;
        real_close(fd);
    }

    return 0;
}

static int _sd_file_output(pcap_dev_t *dev, const void *data, size_t data_len)
{
    pcap_dev_sd_file_state_t *state = dev->sd_state;
    int fd, tmp, res = 0;

    if ((fd = real_open(state->sink_fname,
                        O_WRONLY | O_CREAT | O_APPEND,
                        state->sink_mode)) < 0) {
        DEBUG("_sd_file_output: Unable to open sink \"%s\"\n", state->sink_fname);
        return -errno;
    }
    if ((tmp = real_write(fd, data, data_len)) < 0) {
        DEBUG("_sd_file_output: Unable to write to sink \"%s\"\n", state->sink_fname);
        real_close(fd);
        return -errno;
    }
    res += tmp;
    real_close(fd);

    return res;
}

static int _sd_file_input(pcap_dev_t *dev, pcaprec_hdr_t *data, size_t max_data,
                          timex_t *next)
{
    pcap_dev_sd_file_state_t *state = dev->sd_state;
    int fd, tmp, res = 0;

    if (state->drain_fname == NULL) {
        DEBUG("_sd_file_input: no drain specified, return 0\n");
        /* don't call drain again */
        next->seconds = 0;
        next->microseconds = 0;
        return 0;
    }
    if (max_data < sizeof(pcaprec_hdr_t)) {
        DEBUG("_sd_file_input: data (max_data = %u) does not fit pcaprec_hdr_t\n",
              (unsigned)max_data);
        return -ENOBUFS;
    }

    if ((fd = real_open(state->drain_fname, O_RDONLY)) < 0) {
        DEBUG("_sd_file_input: Unable to open drain \"%s\"\n", state->drain_fname);
        return -errno;
    }
    if ((tmp = (int)real_lseek(fd, state->drain_offset, SEEK_SET)) < 0) {
        DEBUG("_sd_file_input: Could not jump to current offset %u in drain \"%s\"\n",
              (unsigned)state->drain_offset, state->drain_fname);
        real_close(fd);
        return -errno;
    }
    if ((tmp = (int)real_read(fd, data, sizeof(pcaprec_hdr_t))) < 0) {
        DEBUG("_sd_file_init: Unable to read from drain \"%s\"\n", state->drain_fname);
        real_close(fd);
        return -errno;
    }
    res += tmp;
    state->drain_offset += tmp;
    if (data->incl_len > (max_data - sizeof(pcaprec_hdr_t))) {
        DEBUG("_sd_file_input: data (max_data = %u) does not fit record (size: %"
              PRIu32 ")\n", (unsigned)max_data, data->incl_len);
        real_close(fd);
        return -ENOBUFS;
    }
    if ((tmp = (int)real_read(fd, data + 1, data->incl_len)) < 0) {
        DEBUG("_sd_file_init: Unable to read from drain \"%s\"\n", state->drain_fname);
        real_close(fd);
        return -errno;
    }
    res += tmp;
    state->drain_offset += tmp;
    if (real_read(fd, next, sizeof(timex_t)) <= 0) {
        DEBUG("_sd_file_init: Unable to read next timestamp, assuming 0\n");
        next->seconds = 0;
        next->microseconds = 0;
    }
    real_close(fd);
    return res;
}

/** @} */
