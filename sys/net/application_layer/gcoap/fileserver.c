/*
 * Copyright (C) 2020 chrysn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 *
 * @file
 * @brief       CoAP file server implementation
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "checksum/fletcher32.h"
#include "net/gcoap/fileserver.h"
#include "net/gcoap.h"
#include "vfs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/** Maximum length of an expressible path, including the trailing 0 character. */
#define COAPFILESERVER_PATH_MAX (64)

/** Data extracted from a request on a file */
struct requestdata {
    /** 0-terminated expanded file name in the VFS */
    char namebuf[COAPFILESERVER_PATH_MAX];
    uint32_t blocknum2;
    uint32_t etag;
    bool etag_sent;
    uint8_t szx2;
};

/**
 * @brief  Return true if path/name is a directory.
 */
static bool entry_is_dir(char *path, const char *name)
{
    struct stat stat;
    size_t path_len = strlen(path);
    size_t name_len = strlen(name);

    if (path_len + name_len + 1 >= COAPFILESERVER_PATH_MAX) {
        return false;
    }

    /* re-use path buffer, it is already COAPFILESERVER_PATH_MAX bytes long */
    path[path_len] = '/';
    path[path_len + 1] = 0;
    strncat(path, name, COAPFILESERVER_PATH_MAX - 1);

    if (vfs_stat(path, &stat)) {
        DEBUG("vfs_stat(%s) failed\n", path);
    }

    path[path_len] = 0;
    return (stat.st_mode & S_IFMT) == S_IFDIR;
}

static unsigned _count_char(const char *s, char c)
{
    unsigned count = 0;
    while (*s) {
        if (*s == c) {
            ++count;
        }
        ++s;
    }

    return count;
}

/** Build an ETag based on the given file's VFS stat. If the stat fails,
 * returns the error and leaves etag in any state; otherwise there's an etag
 * in the stattag's field */
static int stat_etag(const char *filename, uint32_t *etag)
{
    struct stat stat;
    int err = vfs_stat(filename, &stat);
    if (err < 0) {
        return err;
    }

    /* Normalizing fields whose value can change without affecting the ETag */
    stat.st_nlink = 0;
#if defined(CPU_ESP32) || defined(CPU_ESP8266) || defined(CPU_MIPS_PIC32MX) || defined(CPU_MIPS_PIC32MZ)
    memset(&stat.st_atime, 0, sizeof(stat.st_atime));
#else
    memset(&stat.st_atim, 0, sizeof(stat.st_atim));
#endif

    *etag = fletcher32((void *)&stat, sizeof(stat) / 2);

    return 0;
}

/** Create a CoAP response for a given errno (eg. EACCESS -> 4.03 Forbidden
 * etc., defaulting to 5.03 Internal Server Error) */
static size_t gcoap_fileserver_errno_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, int err)
{
    uint8_t code;
    switch (err) {
    case -EACCES:
        code = COAP_CODE_FORBIDDEN;
        break;
    case -ENOENT:
        code = COAP_CODE_PATH_NOT_FOUND;
        break;
    default:
        code = COAP_CODE_INTERNAL_SERVER_ERROR;
    };
    DEBUG("gcoap_fileserver: Rejecting error %d (%s) as %d.%02d\n", err, strerror(err),
          code >> 5, code & 0x1f);
    return gcoap_response(pdu, buf, len, code);
}

static void _calc_szx2(coap_pkt_t *pdu, size_t reserve, struct requestdata *request)
{
    assert(pdu->payload_len > reserve);
    size_t remaining_length = pdu->payload_len - reserve;
    /* > 0: To not wrap around; if that still won't fit that's later caught in
     * an assertion */
    while ((coap_szx2size(request->szx2) > remaining_length) && (request->szx2 > 0)) {
        request->szx2--;
        request->blocknum2 <<= 1;
    }
}

static ssize_t gcoap_fileserver_file_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                             struct requestdata *request)
{
    uint32_t etag;
    int err = stat_etag(request->namebuf, &etag);
    if (err < 0) {
        return gcoap_fileserver_errno_handler(pdu, buf, len, err);
    }

    if (request->etag_sent && etag == request->etag) {
        gcoap_resp_init(pdu, buf, len, COAP_CODE_VALID);
        coap_opt_add_opaque(pdu, COAP_OPT_ETAG, &etag, sizeof(etag));
        return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
    }

    int fd = vfs_open(request->namebuf, O_RDONLY, 0);
    if (fd < 0) {
        return gcoap_fileserver_errno_handler(pdu, buf, len, fd);
    }

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_opaque(pdu, COAP_OPT_ETAG, &etag, sizeof(etag));
    coap_block_slicer_t slicer;
    _calc_szx2(pdu,
               5 + 1 + 1 /* reserve BLOCK2 size + payload marker + more */,
               request);
    coap_block_slicer_init(&slicer, request->blocknum2, coap_szx2size(request->szx2));
    coap_opt_add_block2(pdu, &slicer, true);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    err = vfs_lseek(fd, slicer.start, SEEK_SET);
    if (err < 0) {
        goto late_err;
    }

    /* That'd only happen if the buffer is too small for even a 16-byte block,
     * or if the above calculations were wrong.
     *
     * Not using payload_len here as that's needlessly underestimating the
     * space by CONFIG_GCOAP_RESP_OPTIONS_BUF
     * */
    assert(pdu->payload + slicer.end - slicer.start <= buf + len);
    bool more = 1;
    int read = vfs_read(fd, pdu->payload, slicer.end - slicer.start + more);
    if (read < 0) {
        goto late_err;
    }
    more = (unsigned)read > slicer.end - slicer.start;
    read -= more;

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

static ssize_t gcoap_fileserver_directory_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                  struct requestdata *request,
                                                  gcoap_fileserver_entry_t *resource)
{
    vfs_DIR dir;
    coap_block_slicer_t slicer;

    int err = vfs_opendir(&dir, request->namebuf);
    if (err != 0) {
        return gcoap_fileserver_errno_handler(pdu, buf, len, err);
    }
    DEBUG("gcoap_fileserver: Serving directory listing\n");

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_LINK);
    _calc_szx2(pdu,
               5 + 1 /* reserve BLOCK2 size + payload marker */,
               request);
    coap_block_slicer_init(&slicer, request->blocknum2, coap_szx2size(request->szx2));
    coap_opt_add_block2(pdu, &slicer, true);
    buf += coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    size_t root_len = resource->root ? strlen(resource->root) : 0;
    const char *root_dir = &request->namebuf[root_len];
    const char *resource_dir = resource->resource;
    size_t root_dir_len = strlen(root_dir);
    size_t resource_dir_len = strlen(resource_dir);

    vfs_dirent_t entry;
    while (vfs_readdir(&dir, &entry) > 0) {
        const char *entry_name = entry.d_name;
        size_t entry_len = strlen(entry_name);
        if (entry_len <= 2 && memcmp(entry_name, "..", entry_len) == 0) {
            /* Up pointers don't work the same way in URI semantics */
            continue;
        }
        bool is_dir = entry_is_dir(request->namebuf, entry_name);

        if (slicer.cur) {
            buf += coap_blockwise_put_char(&slicer, buf, ',');
        }
        buf += coap_blockwise_put_char(&slicer, buf, '<');

        buf += coap_blockwise_put_bytes(&slicer, buf, resource_dir, resource_dir_len);
        buf += coap_blockwise_put_bytes(&slicer, buf, root_dir, root_dir_len);
        buf += coap_blockwise_put_char(&slicer, buf, '/');
        buf += coap_blockwise_put_bytes(&slicer, buf, entry_name, entry_len);
        if (is_dir) {
            buf += coap_blockwise_put_char(&slicer, buf, '/');
        }
        buf += coap_blockwise_put_char(&slicer, buf, '>');
    }

    vfs_closedir(&dir);
    coap_block2_finish(&slicer);

    return (uintptr_t)buf - (uintptr_t)pdu->hdr;
}

ssize_t gcoap_fileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx) {
    gcoap_fileserver_entry_t *entry = ctx;
    struct requestdata request = {
        .etag_sent = false,
        .blocknum2 = 0,
        .szx2 = CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX,
    };

    /** Index in request.namebuf. Must not point at the last entry as that will be
     * zeroed to get a 0-terminated string. */
    size_t namelength = 0;
    uint8_t errorcode = COAP_CODE_INTERNAL_SERVER_ERROR;
    uint8_t strip_remaining = _count_char(entry->resource, '/');

    /* If a root directory for the server was specified, use that */
    if (entry->root && strlen(entry->root) > 1) {
        strncpy(request.namebuf, entry->root, sizeof(request.namebuf));
        namelength = strlen(entry->root);
    }

    bool is_directory = true; /* either no path component at all or trailing '/' */
    coap_optpos_t opt = {
        .offset = coap_get_total_hdr_len(pdu),
    };
    uint8_t *value;
    ssize_t optlen;
    while ((optlen = coap_opt_get_next(pdu, &opt, &value, 0)) != -ENOENT) {

        if (optlen < 0) {
            errorcode = COAP_CODE_BAD_REQUEST;
            goto error;
        }

        switch (opt.opt_num) {
            case COAP_OPT_URI_PATH:
                if (strip_remaining != 0) {
                    strip_remaining -= 1;
                    continue;
                }
                if ((is_directory = (optlen == 0))) { /* '/' */
                    continue;
                }
                if (memchr(value, '\0', optlen) != NULL ||
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
                memcpy(&request.etag, value, sizeof(request.etag));
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
                }
                else {
                    /* Ignoring elective option */
                }
        }
    }

    request.namebuf[namelength] = '\0';

    DEBUG("request: '%s'\n", request.namebuf);

    /* Note to self: As we parse more options than just Uri-Path, we'll likely
     * pass a struct pointer later. So far, those could even be hooked into the
     * resource list, but that'll go away once we parse more options */
    if (is_directory) {
        return gcoap_fileserver_directory_handler(pdu, buf, len, &request, entry);
    }
    else {
        return gcoap_fileserver_file_handler(pdu, buf, len, &request);
    }

error:
    return gcoap_response(pdu, buf, len, errorcode);
}

/** @} */
