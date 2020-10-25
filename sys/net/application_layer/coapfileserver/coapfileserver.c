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

#define ENABLE_DEBUG 1
#include "debug.h"

/** Maximum length of an expressible path, including the trailing 0 character. */
#define COAPFILESERVER_PATH_MAX (64)

/** Constant ETag length */
#define ETAG_LENGTH 8

/** Data extracted from a request on a file */
struct requestdata {
    /** 0-terminated expanded file name in the VFS */
    char namebuf[COAPFILESERVER_PATH_MAX];
    uint32_t blocknum2;
    unsigned int szx2; /* would prefer uint8_t but that's what coap_get_blockopt gives */
    bool etag_sent;
    uint8_t etag[ETAG_LENGTH];
};

/* See stat_etag */
union stattag {
    struct stat stat;
    struct {
        uint8_t etag[ETAG_LENGTH];
        /* This will be as many repetitions of ETAG_LENGTH that together
         * with the original etag field id's larger than the stat */
        uint8_t etag_padding[(sizeof(struct stat) - 1) / ETAG_LENGTH * ETAG_LENGTH];
    };
};

/** Build an ETag based on the given file's VFS stat. If the stat fails,
 * returns the error and leaves stattag in any state; otherwise there's an etag
 * in the stattag's field */
static int stat_etag(char *filename, union stattag *stattag);

/* These are almost but but not quite coap_handler_t -- they require the
 * request to be already fully read and digested into the last argument. */

static ssize_t coapfileserver_file_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, struct requestdata *request);
static ssize_t coapfileserver_directory_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, struct requestdata *request);
/** Create a CoAP response for a given errno (eg. EACCESS -> 4.03 Forbidden
 * etc., defaulting to 5.03 Internal Server Error) */
static size_t coapfileserver_errno_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, int err);
/** Return true if path/name is a directory.
 *
 * Currently assembling the values in a buffer, this could be done without a
 * buffer if an openat / vfs_openat was present.
 * */
static bool entry_is_dir(char *path, char *name);

ssize_t coapfileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx) {
    struct coapfileserver_entry *entry = (struct coapfileserver_entry *)ctx;
    struct requestdata request;
    request.etag_sent = false;
    request.blocknum2 = 0;
    request.szx2 = CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX;
    /** Index in request.namebuf. Must not point at the last entry as that will be
     * zeroed to get a 0-terminated string. */
    size_t namelength = 0;
    bool trailing_slash = false;
    /** If no path component comes along at all, it'll count as a trailing
     * slash no matter the trailing_slash value */
    bool any_component = false;
    uint8_t errorcode = COAP_CODE_INTERNAL_SERVER_ERROR;

    uint8_t strip_remaining = entry->strip_path;
    namelength += request.namebuf - strncpy(request.namebuf, entry->nameprefix, sizeof(request.namebuf) - 1);

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
                if (newlength > sizeof(request.namebuf) - 1) {
                    /* Path too long, therefore can't exist in this mapping */
                    errorcode = COAP_CODE_PATH_NOT_FOUND;
                    goto error;
                }
                request.namebuf[namelength] = '/';
                memcpy(&request.namebuf[namelength] + 1, value, optlen);
                namelength = newlength;
                break;
            case COAP_OPT_ETAG:
                if (optlen != sizeof(request.etag)) {
                    /* Can't be a matching tag, no use in carrying that */
                    continue;
                }
                if (request.etag_sent) {
                    /* We can reasonably only check for a limited sized set,
                     * and it size is 1 here (sending multiple ETags is
                     * possible but rare) */
                    continue;
                }
                request.etag_sent = true;
                memcpy(request.etag, value, sizeof(request.etag));
                break;
            case COAP_OPT_BLOCK2:
                /* Could be more efficient now that we already know where it
                 * is, but meh */
                coap_get_blockopt(pdu, COAP_OPT_BLOCK2, &request.blocknum2, &request.szx2);
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

    request.namebuf[namelength] = '\0';
    bool is_directory = trailing_slash | !any_component;

    /* Note to self: As we parse more options than just Uri-Path, we'll likely
     * pass a struct pointer later. So far, those could even be hooked into the
     * resource list, but that'll go away once we parse more options */
    if (is_directory) {
        return coapfileserver_directory_handler(pdu, buf, len, &request);
    } else {
        return coapfileserver_file_handler(pdu, buf, len, &request);
    }

error:
    return gcoap_response(pdu, buf, len, errorcode);
}

static ssize_t coapfileserver_file_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, struct requestdata *request)
{
    int err;

    union stattag stattag;
    err = stat_etag(request->namebuf, &stattag);
    if (err < 0)
        return coapfileserver_errno_handler(pdu, buf, len, err);

    if (request->etag_sent && memcmp(stattag.etag, request->etag, ETAG_LENGTH) == 0) {
        gcoap_resp_init(pdu, buf, len, COAP_CODE_VALID);
        coap_opt_add_opaque(pdu, COAP_OPT_ETAG, stattag.etag, ETAG_LENGTH);
        return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
    }

    int fd = vfs_open(request->namebuf, O_RDONLY, 0);
    if (fd < 0)
        return coapfileserver_errno_handler(pdu, buf, len, fd);

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_opaque(pdu, COAP_OPT_ETAG, stattag.etag, ETAG_LENGTH);
    /* To best see how this works set CONFIG_GCAOP_PDU_BUF_SIZE to 532 or 533.
     * If we did a sharper estimation (factoring in the block2 size option with
     * the current blockum), we'd even pack 512 bytes into 530 until block
     * numbers get large enough to eat another byte, which is when the block
     * size would decrease in-flight. */
    size_t remaining_length = len - (pdu->payload - buf);
    remaining_length -= 5; /* maximum block2 option usable in nanocoap */
    remaining_length -= 1; /* payload marker */
    /* > 0: To not wrap around; if that still won't fit that's later caught in
     * an assertion */
    while (coap_szx2size(request->szx2) > remaining_length && request->szx2 > 0) {
        request->szx2 --;
        request->blocknum2 <<= 1;
    }
    coap_block_slicer_t slicer;
    coap_block_slicer_init(&slicer, request->blocknum2, coap_szx2size(request->szx2));
    coap_opt_add_block2(pdu, &slicer, true);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    err = vfs_lseek(fd, slicer.start, SEEK_SET);
    if (err < 0)
        goto late_err;

    /* That'd only happen if the buffer is too small for even a 16-byte block,
     * or if the above calculations were wrong.
     *
     * Not using payload_len here as that's needlessly underestimating the
     * space by CONFIG_GCOAP_RESP_OPTIONS_BUF
     * */
    assert(pdu->payload + slicer.end - slicer.start <= buf + len);
    int read = vfs_read(fd, pdu->payload, slicer.end - slicer.start);
    if (err < 0)
        goto late_err;

    uint8_t morebuf;
    int more = vfs_read(fd, &morebuf, 1);
    if (err < 0)
        goto late_err;

    vfs_close(fd);

    slicer.cur = slicer.end + more;
    coap_block2_finish(&slicer);

    if (read == 0) {
        /* Rewind to clear payload marker */
        read -= 1;
    }

    return resp_len + read;

late_err:
    vfs_close(fd);
    coap_hdr_set_code(pdu->hdr, COAP_CODE_INTERNAL_SERVER_ERROR);
    return coap_get_total_hdr_len(pdu);
}

static ssize_t coapfileserver_directory_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, struct requestdata *request)
{
    /**
     * ToDo:
     *
     * * Blockwise
     *
     */
    vfs_DIR dir;

    int err = vfs_opendir(&dir, request->namebuf);
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
        char *entry_name = (char*)entry.d_name;
        /* Ignore leading slashes presented by some file systems; see
         * https://github.com/RIOT-OS/RIOT/issues/14635 */
        while (entry_name[0] == '/') {
            entry_name += 1;
        }
        size_t entry_len = strlen(entry_name);
        if (entry_len <= 2 && memcmp(entry_name, "..", entry_len) == 0) {
            /* Up pointers don't work the same way in URI semantics */
            continue;
        }
        bool is_dir = entry_is_dir(request->namebuf, entry_name);
        /* maybe ",", "<>", and the length */
        ssize_t need_bytes = (payload_cursor == 0 ? 0 : 1) + 2 + entry_len + (is_dir ? 1 : 0);
        if (payload_cursor + need_bytes > pdu->payload_len) {
            /* Without blockwise, this is the best approximation we can do */
            DEBUG("coapfileserver: Directory listing truncated\n");
            break;
        }
        if (payload_cursor != 0) {
            pdu->payload[payload_cursor++] = ',';
        }
        pdu->payload[payload_cursor++] = '<';
        memcpy(&pdu->payload[payload_cursor], entry_name, entry_len);
        payload_cursor += entry_len;
        if (is_dir) {
            pdu->payload[payload_cursor++] = '/';
        }
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

static int stat_etag(char *filename, union stattag *stattag)
{
    int err;

    memset(stattag, 0, sizeof(union stattag));
    err = vfs_stat(filename, &stattag->stat);
    if (err < 0) {
        /* Some file systems, like  only implement fstat. FIXME: run on file
         * handles all the time and only use fstat. (Not trivial because it
         * needs going through all filesystems to check whether there isn't one
         * that only does stat and not fstat) */
        int fd = vfs_open(filename, O_RDONLY, 0);
        if (fd < 0) {
            return fd;
        }
        err = vfs_fstat(fd, &stattag->stat);
        vfs_close(fd);

        return err;
    }

    /* Normalizing fields whose value can change without affecting the ETag */
    stattag->stat.st_nlink = 0;
    memset(&stattag->stat.st_atim, 0, sizeof(stattag->stat.st_atim));

    /* Build a compact ETag by XORing the stat onto itself */
    for (unsigned int i = 0; i < sizeof(stattag->etag_padding) / sizeof(stattag->etag); ++i) {
        for (int j = 0; j < ETAG_LENGTH; ++j) {
            stattag->etag[j] ^= stattag->etag_padding[i * ETAG_LENGTH + j];
        }
    }

    return 0;
}

static bool entry_is_dir(char *path, char *name)
{
    /* FIXME have an openat or fstatat
     *
     * or just build the long name outside because it's printed later, or use
     * it from the printed buffer? */
    char buf[COAPFILESERVER_PATH_MAX];
    /* snprintf may be expensive, but without a strlcpy it's a pain to do
     * manually */
    int printed = snprintf(buf, sizeof(buf), "%s/%s", path, name);
    if (printed <= 0) {
        /* Returning true on error as a slash in a file name is more likely to
         * cause suspicion than the lack of one at a directory */
        return true;
    }

    struct stat stat;
    memset(&stat, 0, sizeof(stat));
    int err = vfs_stat(buf, &stat);
    if (err != 0) {
        int fd = vfs_open(buf, O_RDONLY, 0);
        if (fd < 0) {
            return true;
        }
        err = vfs_fstat(fd, &stat);
        vfs_close(fd);

        if (err != 0) {
            return true;
        }
    }
    return (stat.st_mode & S_IFMT) == S_IFDIR;
}
