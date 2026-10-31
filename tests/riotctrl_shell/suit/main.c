/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the basic shell commands always provided.
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "shell.h"
#include "suit/storage.h"
#include "suit/transport/mock.h"

static const uint8_t _buf1_bin[] = "foo";
static const uint8_t _buf2_bin[] = "bar";

const suit_transport_mock_payload_t payloads[] = {
    {
        .buf = _buf1_bin,
        .len = sizeof(_buf1_bin),
    },
    {
        .buf = _buf2_bin,
        .len = sizeof(_buf2_bin),
    }
};

const size_t num_payloads = ARRAY_SIZE(payloads);

static const uint8_t _manifest_mock[] = "This is just an unparsable mock manifest.";

int suit_storage_get_highest_seq_no(uint32_t *seq_no)
{
    *seq_no = 0xc0ffee;
    return SUIT_OK;
}

suit_storage_t *suit_storage_find_by_component(const suit_manifest_t *manifest,
                                               const suit_component_t *component)
{
    (void)manifest;
    (void)component;
    return NULL;
}

int riotboot_flashwrite_invalidate_latest(void)
{
    return 0;
}

ssize_t testsuite_suit_manifest_get(uint8_t *buf, size_t buf_len)
{
    if (sizeof(_manifest_mock) > buf_len) {
        return -ENOBUFS;
    }
    memcpy(buf, _manifest_mock, sizeof(_manifest_mock));
    return sizeof(_manifest_mock);
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.suit test");

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
