/*
 * Copyright (C) 2020 chrysn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <assert.h>

#include <net/coapfileserver.h>
#include <net/gcoap.h>
#include <vfs.h>
#include <fcntl.h>
#include <error.h>

#define ENABLE_DEBUG 0
#include "debug.h"

/** Maximum length of an expressible path, including the trailing 0 character. */
#define COAPFILESERVER_PATH_MAX (64)

static ssize_t coapfileserver_file_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);
static ssize_t coapfileserver_directory_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);
/** Create a CoAP response for a given errno (eg. EACCESS -> 4.03 Forbidden
 * etc., defaulting to 5.03 Internal Server Error) */
static size_t coapfileserver_errno_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, int err);

ssize_t coapfileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx) {
    struct coapfileserver_entry *entry = (struct coapfileserver_entry *)ctx;
    char namebuf[COAPFILESERVER_PATH_MAX];
    /** Index in namebuf. Must not point at the last entry as that will be
     * zeroed to get a 0-terminated string. */
    size_t namelength = 0;
    bool trailing_slash = false;
    /** If no path component comes along at all, it'll count as a trailing
     * slash no matter the trailing_slash value */
    bool any_component = false;
    uint8_t errorcode = COAP_CODE_INTERNAL_SERVER_ERROR;

    uint8_t strip_remaining = entry->strip_path;
    namelength += namebuf - strncpy(namebuf, entry->nameprefix, sizeof(namebuf) - 1);

    coap_optpos_t opt;
    bool is_first = true;
    uint8_t *value;
    ssize_t optlen;
    while ((optlen = coap_opt_get_next(pdu, &opt, &value, is_first)) !=
            -ENOENT)
    {
        if (optlen < 0) {
            errorcode = COAP_CODE_BAD_REQUEST;
            goto error;
        }

        is_first = false;
        switch (opt.opt_num) {
            case COAP_OPT_URI_PATH:
                if (strip_remaining != 0) {
                    strip_remaining -= 1;
                    continue;
                }
                if (trailing_slash) {
                    errorcode = COAP_CODE_BAD_REQUEST;
                    goto error;
                }
                any_component = true;
                if (optlen == 0) {
                    trailing_slash = true;
                    continue;
                }
                if (memchr(value, '0', optlen) != NULL ||
                        memchr(value, '/', optlen) != NULL) {
                    /* Path can not be expressed in the file system */
                    errorcode = COAP_CODE_PATH_NOT_FOUND;
                    goto error;
                }
                size_t newlength = namelength + 1 + optlen;
                if (newlength > sizeof(namebuf) - 1) {
                    /* Path too long, therefore can't exist in this mapping */
                    errorcode = COAP_CODE_PATH_NOT_FOUND;
                    goto error;
                }
                namebuf[namelength] = '/';
                memcpy(&namebuf[namelength] + 1, value, optlen);
                namelength = newlength;
                break;
            default:
                if (opt.opt_num & 1) {
                    errorcode = COAP_CODE_BAD_REQUEST;
                    goto error;
                } else {
                    /* Ignoring elective option */
                }
        }
    }

    namebuf[namelength] = '\0';
    bool is_directory = trailing_slash | !any_component;

    /* Note to self: As we parse more options than just Uri-Path, we'll likely
     * pass a struct pointer later. So far, those could even be hooked into the
     * resource list, but that'll go away once we parse more options */
    if (is_directory) {
        return coapfileserver_directory_handler(pdu, buf, len, namebuf);
    } else {
        return coapfileserver_file_handler(pdu, buf, len, namebuf);
    }

error:
    return gcoap_response(pdu, buf, len, errorcode);
}

static ssize_t coapfileserver_file_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx) {
    /**
     * ToDo:
     *
     * * Blockwise
     *
     * * ETag
     *
     * * Error handling on late read errors
     */
    const char *name = (char *)ctx;

    int fd = vfs_open(name, O_RDONLY, 0);
    if (fd < 0)
        return coapfileserver_errno_handler(pdu, buf, len, fd);

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    int read = vfs_read(fd, pdu->payload, pdu->payload_len);
    assert(read >= 0); /* Can't rewind PDU yet */

    vfs_close(fd);

    if (read == 0) {
        /* Rewind to clear payload marker */
        read -= 1;
    }

    return resp_len + read;
}

static ssize_t coapfileserver_directory_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx) {
    /**
     * ToDo:
     *
     * * Produce actual link format (considering the 'origin' part, ie. always
     *   giving full paths including path-so-far from the request PDU), or
     *   serve CoRAL right away
     *
     * * Blockwise
     *
     * * Directories don't have their trailing slashes yet
     */
    const char *name = (char *)ctx;
    vfs_DIR dir;

    int err = vfs_opendir(&dir, name);
    if (err != 0) {
        return coapfileserver_errno_handler(pdu, buf, len, err);
    }
    DEBUG("coapfileserver: Serving directory listing\n");

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_LINK);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    vfs_dirent_t entry;
    ssize_t payload_cursor = 0;
    while (vfs_readdir(&dir, &entry) > 0) {
        size_t entry_len = strlen((char*)&entry.d_name);
        /* maybe ",", "<>", and the length without leading slash */
        ssize_t need_bytes = (payload_cursor == 0 ? 0 : 1) + 2 + entry_len - 1;
        if (payload_cursor + need_bytes > pdu->payload_len) {
            /* Without blockwise, this is the best approximation we can do */
            DEBUG("coapfileserver: Directory listing truncated\n");
            break;
        }
        if (payload_cursor != 0) {
            pdu->payload[payload_cursor++] = ',';
        }
        pdu->payload[payload_cursor++] = '<';
        memcpy(&pdu->payload[payload_cursor], &entry.d_name[1], entry_len - 1);
        payload_cursor += entry_len - 1;
        pdu->payload[payload_cursor++] = '>';
    }
    vfs_closedir(&dir);

    if (payload_cursor == 0) {
        /* Rewind to clear payload marker */
        payload_cursor -= 1;
    }

    return resp_len + payload_cursor;
}

static size_t coapfileserver_errno_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, int err)
{
    uint8_t code;
    switch (err) {
        case -EACCES:
            code = COAP_CODE_FORBIDDEN; break;
        case -ENOENT:
            code = COAP_CODE_PATH_NOT_FOUND; break;
        default:
            code = COAP_CODE_INTERNAL_SERVER_ERROR;
    };
    DEBUG("coapfileserver: Rejecting error %d (%s) as %d.%02d\n", err, strerror(err), code >> 5, code & 0x1f);
    return gcoap_response(pdu, buf, len, code);
}
