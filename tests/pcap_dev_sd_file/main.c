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
 *
 * @author   Martine Lenders <mlenders@inf.fu-berlin.de
 */

#include <stdio.h>

#include "od.h"
#include "pcap_dev/sd_file.h"
#include "timex.h"

#define DRAIN_TEST_BUFSIZE              (sizeof(pcaprec_hdr_t) + 128)
#define SINK_TEST_DATA_LEN              (3)
#define SINK_TEST_DATA_PAYLOAD_LENGTH   (43)

static const struct {
    pcaprec_hdr_t hdr;
    uint8_t payload[SINK_TEST_DATA_PAYLOAD_LENGTH];
} sink_test_data[SINK_TEST_DATA_LEN] = {
    {
        .hdr = {
            .timestamp = { .seconds = 1457972357, .microseconds = 623955 },
            .incl_len = SINK_TEST_DATA_PAYLOAD_LENGTH,
            .orig_len = SINK_TEST_DATA_PAYLOAD_LENGTH,
        },
        .payload = {
            0x41, 0xd8, 0x08, 0x77, 0x07, 0xff, 0xff, 0x12,
            0x40, 0xab, 0x8b, 0x54, 0x4e, 0x16, 0x12, 0x7b,
            0x3b, 0x3a, 0x02, 0x85, 0x00, 0x25, 0xd5, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x02, 0x12, 0x16, 0x4e,
            0x54, 0x8b, 0xab, 0x40, 0x12, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00,
        },
    },
    {
        .hdr = {
            .timestamp = { .seconds = 1457972367, .microseconds = 415096 },
            .incl_len = SINK_TEST_DATA_PAYLOAD_LENGTH,
            .orig_len = SINK_TEST_DATA_PAYLOAD_LENGTH,
        },
        .payload = {
            0x41, 0xd8, 0x0f, 0x77, 0x07, 0xff, 0xff, 0x12,
            0x40, 0xab, 0x8b, 0x54, 0x4e, 0x16, 0x12, 0x7b,
            0x3b, 0x3a, 0x02, 0x85, 0x00, 0x25, 0xd5, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x02, 0x12, 0x16, 0x4e,
            0x54, 0x8b, 0xab, 0x40, 0x12, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00,
        },
    },
    {
        .hdr = {
            .timestamp = { .seconds = 1457972380, .microseconds = 135370 },
            .incl_len = SINK_TEST_DATA_PAYLOAD_LENGTH,
            .orig_len = SINK_TEST_DATA_PAYLOAD_LENGTH,
        },
        .payload = {
            0x41, 0xd8, 0x12, 0x77, 0x07, 0xff, 0xff, 0x12,
            0x40, 0xab, 0x8b, 0x54, 0x4e, 0x16, 0x12, 0x7b,
            0x3b, 0x3a, 0x02, 0x85, 0x00, 0x25, 0xd5, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x02, 0x12, 0x16, 0x4e,
            0x54, 0x8b, 0xab, 0x40, 0x12, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00,
        },
    },
};
static uint8_t drain_test_buf[DRAIN_TEST_BUFSIZE];

static const pcap_hdr_t pcap_hdr = {
    .magic_number = PCAP_MAGIC_NUMBER,
    .version_major = PCAP_VERSION_MAJOR,
    .version_minor = PCAP_VERSION_MINOR,
    .this_zone = 0,
    .sigfigs = 0,
    .snaplen = 0xffffU,
    .net = PCAP_NET_IEEE802154_NOFCS,
};
static pcap_dev_sd_file_state_t state = {
    .sink_fname = "sink.pcap",
    .drain_fname = "drain.pcap",
    .drain_offset = 0,
    .drain_swapped = false,
    .sink_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH,
};
static pcap_dev_t dev;

static void test_sink(void)
{
    for (int i = 0; i < SINK_TEST_DATA_LEN; i++) {
        if (pcap_dev_sd_file.output(&dev, &sink_test_data[i].hdr,
                                    sizeof(sink_test_data[i].hdr)) < 0) {
            printf("Error putting frame %i into sink\n", i);
            break;
        }
        if (pcap_dev_sd_file.output(&dev, &sink_test_data[i].payload,
                                    sizeof(sink_test_data[i].payload)) < 0) {
            printf("Error putting frame %i into sink\n", i);
            break;
        }
    }
    puts("");
    puts("Sink test done. Please check sink.pcap with a sniffing tool (e.g. wireshark).");
    puts("");
    puts("You should see 3 Router Solicitations from 12:16:4e:54:8b:ab:40:12 with");
    puts("fe80::1016:4e54:8bab:4012 as source and ff02::2 as destination");
    puts("");
}

static void test_drain(void)
{
    unsigned count = 0;
    timex_t next_time = { 1, 0 };

    while ((next_time.seconds != 0) || (next_time.microseconds != 0)) {
        pcaprec_hdr_t *hdr = (pcaprec_hdr_t *)drain_test_buf;
        int res;

        if ((res = pcap_dev_sd_file.input(&dev, hdr, sizeof(drain_test_buf),
                                          &next_time)) > 0) {
            uint8_t *data = &drain_test_buf[sizeof(pcaprec_hdr_t)];
            char time_str[TIMEX_MAX_STR_LEN];

            puts("");
            printf("Got packet from drain (next_time: %s):\n",
                   timex_to_str(next_time, time_str));
            printf("Time: %s\n", timex_to_str(hdr->timestamp, time_str));
            printf("incl_len: %" PRIu32 "\n", hdr->incl_len);
            printf("orig_len: %" PRIu32 "\n", hdr->orig_len);
            puts("data (check with e.g. wireshark):");
            od_hex_dump(data, hdr->incl_len, OD_WIDTH_DEFAULT);
            count++;
        }
        else if (res != 0) {
            puts("Error getting packet from drain");
            break;
        }
    }
    puts("");
    printf("Got %u packets from drain\n\n", count);
}

int main(void)
{
    pcap_dev_opt_fn_t null[] = { NULL };

    pcap_dev_setup(&dev, &pcap_dev_sd_file, &state, 0, NULL, 0, pcap_hdr.net,
                   null, null);
    pcap_dev_sd_file.init(&dev, &pcap_hdr);
    test_sink();
    test_drain();
}

/** @} */
