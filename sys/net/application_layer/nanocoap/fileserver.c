/*
 * SPDX-FileCopyrightText: 2020 chrysn
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "net/nanocoap/fileserver.h"
#include "vfs.h"

#if MODULE_VFS_UTIL
#  include "vfs_util.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/** Maximum length of an expressible path, including the trailing 0 character. */
#define COAPFILESERVER_PATH_MAX (64)

/**
 * @brief   fileserver event callback, only used with `nanocoap_fileserver_callback`
 */
static nanocoap_fileserver_event_handler_t _event_cb;

/**
 * @brief   fileserver event callback context, only used with `nanocoap_fileserver_callback`
 */
static void *_event_ctx;

/**
 * @brief   fileserver event mutex, protects event cb and event ctx from concurrent access
 */
static mutex_t _event_mtx;

/**
 * @brief   Structure holding information about present options in a request
 */
struct requestoptions {
    uint32_t etag;                  /**< Etag value in an Etag option */
    uint32_t if_match;              /**< Etag value in an If-Match option */
    char if_match_len;              /**< Length of the Etag in an If-Match option */
    struct {
        bool if_match        :1;    /**< Request carries an If-Match option */
        bool if_none_match   :1;    /**< Request carries an If-None-Match option */
        bool etag            :1;    /**< Request carries an Etag option */
        bool block2          :1;    /**< Request carries a Block2 option */
        bool block1          :1;    /**< Request carries a Block1 option */
        bool size2           :1;    /**< Request carries a Size2 option */
    } exists;                       /**< Structure holding flags of present request options */
};

/** Data extracted from a request on a file */
struct requestdata {
    /** 0-terminated expanded file name in the VFS */
    char namebuf[COAPFILESERVER_PATH_MAX];
    struct requestoptions options;
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

    /* reuse path buffer, it is already COAPFILESERVER_PATH_MAX bytes long */
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
static void stat_etag(struct stat *stat, uint32_t *etag)
{
    /* Normalizing fields whose value can change without affecting the ETag */
    stat->st_nlink = 0;
#if defined(CPU_ESP32) || defined(CPU_ESP8266)
    memset(&stat->st_atime, 0, sizeof(stat->st_atime));
#else
    memset(&stat->st_atim, 0, sizeof(stat->st_atim));
#endif
    *etag = fletcher32((void *)stat, sizeof(*stat) / 2);
}

/** Create a CoAP response for a given errno (eg. EACCESS -> 4.03 Forbidden
 * etc., defaulting to 5.03 Internal Server Error), or interpret a positive
 * value for err as a CoAP response code */
static size_t _error_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, int err)
{
    uint8_t code = err;
    if (err < 0) {
        switch (err) {
            case -EACCES:
                code = COAP_CODE_FORBIDDEN;
                break;
            case -ENOENT:
                code = COAP_CODE_PATH_NOT_FOUND;
                break;
            case -EBADMSG:
                code = COAP_CODE_BAD_REQUEST;
                break;
            default:
                code = COAP_CODE_INTERNAL_SERVER_ERROR;
        };
        DEBUG("nanocoap_fileserver: Rejecting error %d (%s) as %d.%02d\n", err, strerror(err),
              code >> 5, code & 0x1f);
    }

    coap_builder_t resp;
    int retval = coap_builder_init_reply(&resp, buf, len, pdu, code);
    if (retval) {
        return retval;
    }

    return coap_builder_msg_size(&resp);
}

static inline void _event_file(nanocoap_fileserver_event_t event, struct requestdata *request)
{
    if (!IS_USED(MODULE_NANOCOAP_FILESERVER_CALLBACK)) {
        return;
    }

    mutex_lock(&_event_mtx);
    nanocoap_fileserver_event_ctx_t ctx = {
        .path = request->namebuf,
        .user_ctx = _event_ctx,
    };

    nanocoap_fileserver_event_handler_t cb = _event_cb;
    mutex_unlock(&_event_mtx);

    if (cb) {
        cb(event, &ctx);
    }
}

static ssize_t _get_file(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                         struct requestdata *request)
{
    int err;
    uint32_t etag, size_total;

    {
        struct stat stat;
        if ((err = vfs_stat(request->namebuf, &stat)) < 0) {
            return _error_handler(pdu, buf, len, err);
        }
        size_total = stat.st_size;
        stat_etag(&stat, &etag);
    }

    if (request->options.exists.if_match &&
        memcmp(&etag, &request->options.if_match, request->options.if_match_len) != 0)
    {
        return _error_handler(pdu, buf, len, COAP_CODE_PRECONDITION_FAILED);
    }

    if (request->options.exists.etag &&
        !memcmp(&etag, &request->options.etag, sizeof(etag)))
    {
        coap_builder_t resp;
        int retval = coap_builder_init_reply(&resp, buf, len, pdu, COAP_CODE_VALID);
        if (retval) {
            return retval;
        }
        coap_opt_put_etag(&resp, &etag, sizeof(etag));
        return coap_builder_msg_size(&resp);
    }

    int fd = vfs_open(request->namebuf, O_RDONLY, 0);
    if (fd < 0) {
        return _error_handler(pdu, buf, len, fd);
    }

    coap_block_slicer_t slicer;
    err = coap_block2_init(pdu, &slicer);
    if (err) {
        return err;
        goto late_err;
    }

   coap_builder_t resp;
    err = coap_builder_init_reply(&resp, buf, len, pdu, COAP_CODE_CONTENT);
    if (err) {
        vfs_close(fd);
        return err;
    }

    coap_opt_put_etag(&resp, &etag, sizeof(etag));
    coap_opt_put_block2(&resp, &slicer);

    if (request->options.exists.block2) {
        coap_opt_put_size2(&resp, size_total);
    }

    err = vfs_lseek(fd, slicer.start, SEEK_SET);
    if (err < 0) {
        goto late_err;
    }

    if (slicer.start == 0) {
        _event_file(NANOCOAP_FILESERVER_GET_FILE_START, request);
    }

    size_t bytes_to_read = slicer.end - slicer.start;

    /* That'd only happen if the buffer is too small for even a 16-byte block,
     * or if the above calculations were wrong.
     *
     * It is handled "gracefully" in production builds, but in development
     * builds we can aid debugging of 5.00 Internal Server errors with a blown
     * assertion here.
     */
    assert(bytes_to_read <= coap_builder_buf_remaining(&resp));
    void *pld_ptr = coap_builder_allocate_payload(&resp, bytes_to_read);
    if (!pld_ptr) {
        goto late_err;
    }

    int read = vfs_read(fd, pld_ptr, bytes_to_read);
    if (read < 0) {
        goto late_err;
    }

    vfs_close(fd);

    /* HACK: Normally, we use coap_blockwise_put_bytes(), but here we bypass
     *       the API to not have to read the whole file to RAM first. We advance
     *       the state by hand for now until the API is extended to our use case.
     */
    slicer.cur = size_total;
    bool more = coap_block2_finish(&slicer);

    if (!more) {
        _event_file(NANOCOAP_FILESERVER_GET_FILE_END, request);
        /* HACK: Allocated for a full block in resp, but might only have filled
         *       it partually. */
        resp.pos -= (bytes_to_read - read);
    }

    return coap_builder_msg_size(&resp);

late_err:
    vfs_close(fd);
    return _error_handler(pdu, buf, len, -EINVAL);
}

#if IS_USED(MODULE_NANOCOAP_FILESERVER_PUT)
static ssize_t _put_file(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                         struct requestdata *request)
{
    int ret, fd;
    uint32_t etag;
    struct stat stat;
    coap_block1_t block1 = {0};
    bool create = (vfs_stat(request->namebuf, &stat) == -ENOENT);
    if (create) {
        /* While a file 'f' is initially being created,
           we save the partial content in '.f' and rename it afterwards */
        if (!(ret = strlen(request->namebuf)) || (unsigned)ret >= sizeof(request->namebuf) - 1) {
            /* need one more char '.' */
            return _error_handler(pdu, buf, len, -ENOBUFS);
        }
        char *file = strrchr(request->namebuf, '/');
        memmove(file + 2, file + 1, strlen(file + 1));
        *(file + 1) = '.';
    }
    if (request->options.exists.block1 && !coap_get_block1(pdu, &block1)) {
        ret = COAP_CODE_BAD_OPTION;
        goto unlink_on_error;
    }
    if (block1.blknum == 0) {
        /* "If-None-Match only works correctly on Block1 requests with (NUM=0)
           and MUST NOT be used on Block1 requests with NUM != 0." */
        if (request->options.exists.if_none_match && !create) {
            ret = COAP_CODE_PRECONDITION_FAILED;
            goto unlink_on_error;
        }

        _event_file(NANOCOAP_FILESERVER_PUT_FILE_START, request);
    }
    if (request->options.exists.if_match) {
        stat_etag(&stat, &etag); /* Etag before write */
        if (create || memcmp(&etag, &request->options.if_match, request->options.if_match_len)) {
            ret = COAP_CODE_PRECONDITION_FAILED;
            goto unlink_on_error;
        }
    }
    ret = O_WRONLY;
    ret |= (create ? O_CREAT | O_APPEND : 0);
    if ((fd = vfs_open(request->namebuf, ret, 0666)) < 0) {
        ret = fd;
        goto unlink_on_error;
    }
    if (create) {
        if ((ret = vfs_lseek(fd, 0, SEEK_END)) < 0) {
            goto close_on_error;
        }
        if (block1.offset < (unsigned)ret) {
            /* ignore duplicate packet */
            create = false; /* don't delete file */
            ret = COAP_CODE_CONTINUE;
            goto close_on_error;
        }
        if (block1.offset > (unsigned)ret) {
            /* expect block to be in the correct order during initial creation */
            ret = COAP_CODE_REQUEST_ENTITY_INCOMPLETE;
            goto close_on_error;
        }
    }
    else {
        if (block1.offset > (unsigned)stat.st_size) {
            /* after initial file creation, random access is fine,
                as long as it does not result in holes in the file */
            ret = COAP_CODE_REQUEST_ENTITY_INCOMPLETE;
            goto close_on_error;
        }
        if ((ret = vfs_lseek(fd, block1.offset, SEEK_SET)) < 0) {
            goto close_on_error;
        }
    }
    if ((ret = vfs_write(fd, pdu->payload, pdu->payload_len)) < 0 ||
        (unsigned)ret != pdu->payload_len) {
        goto close_on_error;
    }
    vfs_close(fd);
    coap_builder_t resp;
    if (!block1.more) {
        if ((ret = vfs_stat(request->namebuf, &stat)) < 0) {
            goto unlink_on_error;
        }
        if (create) {
            char path[COAPFILESERVER_PATH_MAX];
            strcpy(path, request->namebuf);
            char *file = strrchr(path, '/');
            memmove(file + 1, file + 2, strlen(file + 2) + 1);
            if ((ret = vfs_rename(request->namebuf, path)) < 0) {
                goto unlink_on_error;
            }
        }

        _event_file(NANOCOAP_FILESERVER_PUT_FILE_END, request);

        stat_etag(&stat, &etag); /* Etag after write */
        ret = coap_builder_init_reply(&resp, buf, len, pdu, create ? COAP_CODE_CREATED : COAP_CODE_CHANGED);
        if (ret) {
            return ret;
        }
        coap_opt_put_etag(&resp, &etag, sizeof(etag));
    }
    else {
        ret = coap_builder_init_reply(&resp, buf, len, pdu, COAP_CODE_CONTINUE);
        if (ret) {
            return ret;
        }
        block1.more = true; /* resource is created atomically */
        coap_opt_put_block1_control(&resp, &block1);
    }
    return coap_builder_msg_size(&resp);

close_on_error:
    vfs_close(fd);
unlink_on_error:
    if (create) {
        vfs_unlink(request->namebuf);
    }
    return _error_handler(pdu, buf, len, ret);
}
#endif

#if IS_USED(MODULE_NANOCOAP_FILESERVER_DELETE)
static ssize_t _delete_file(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                            struct requestdata *request)
{
    int ret;
    uint32_t etag;
    struct stat stat;
    if ((ret = vfs_stat(request->namebuf, &stat)) < 0) {
        return _error_handler(pdu, buf, len, ret);
    }
    if (request->options.exists.if_match && request->options.if_match_len) {
        stat_etag(&stat, &etag);
        if (memcmp(&etag, &request->options.if_match, request->options.if_match_len)) {
            return _error_handler(pdu, buf, len, COAP_CODE_PRECONDITION_FAILED);
        }
    }

    _event_file(NANOCOAP_FILESERVER_DELETE_FILE, request);

    if ((ret = vfs_unlink(request->namebuf)) < 0) {
        return _error_handler(pdu, buf, len, ret);
    }

    coap_builder_t resp;
    ret = coap_builder_init_reply(&resp, buf, len, pdu, COAP_CODE_DELETED);
    if (ret) {
        return ret;
    }
    return coap_builder_msg_size(&resp);
}
#endif

static ssize_t nanocoap_fileserver_file_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                             struct requestdata *request)
{
    switch (coap_get_method(pdu)) {
        case COAP_METHOD_GET:
            return _get_file(pdu, buf, len, request);
#if IS_USED(MODULE_NANOCOAP_FILESERVER_PUT)
        case COAP_METHOD_PUT:
            return _put_file(pdu, buf, len, request);
#endif
#if IS_USED(MODULE_NANOCOAP_FILESERVER_DELETE)
        case COAP_METHOD_DELETE:
            return _delete_file(pdu, buf, len, request);
#endif
        default:
            return _error_handler(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
    }
}

static ssize_t _get_directory(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                              struct requestdata *request,
                              const char *root, const char* resource_dir)
{
    int err;
    vfs_DIR dir;
    coap_block_slicer_t slicer;
    coap_block1_t block2 = { .szx = CONFIG_NANOCOAP_BLOCK_SIZE_MAX };
    if (request->options.exists.block2 && !coap_get_block2(pdu, &block2)) {
        return _error_handler(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }
    if ((err = vfs_opendir(&dir, request->namebuf)) < 0) {
        return _error_handler(pdu, buf, len, err);
    }
    if (request->options.exists.if_match && request->options.if_match_len) {
        return _error_handler(pdu, buf, len, COAP_CODE_PRECONDITION_FAILED);
    }
    DEBUG("nanocoap_fileserver: Serving directory listing\n");

    err = coap_block2_init(pdu, &slicer);
    if (err) {
        return _error_handler(pdu, buf, len, err);
    }
    coap_builder_t resp;
    err = coap_builder_init_reply(&resp, buf, len, pdu, COAP_CODE_CONTENT);
    if (err) {
        vfs_closedir(&dir);
        return err;
    }
    coap_opt_put_ct(&resp, COAP_FORMAT_LINK);
    coap_opt_put_block2(&resp, &slicer);

    size_t root_len = root ? strlen(root) : 0;
    const char *root_dir = &request->namebuf[root_len];
    size_t root_dir_len = strlen(root_dir);
    size_t resource_dir_len = strlen(resource_dir);

    vfs_dirent_t entry;
    while (vfs_readdir(&dir, &entry) > 0) {
        const char *entry_name = entry.d_name;
        size_t entry_len = strlen(entry_name);
        if (*entry_name == '.') {
            /* Exclude everything that starts with '.' */
            continue;
        }
        bool is_dir = entry_is_dir(request->namebuf, entry_name);

        if (slicer.cur) {
            coap_blockwise_put_char(&resp, &slicer, ',');
        } else {
            /* no payload written yet - set payload marker */
            coap_builder_add_payload_marker(&resp);
        }
        coap_blockwise_put_char(&resp, &slicer, '<');
        coap_blockwise_put_bytes(&resp, &slicer, resource_dir, resource_dir_len);
        coap_blockwise_put_bytes(&resp, &slicer, root_dir, root_dir_len);
        coap_blockwise_put_char(&resp, &slicer, '/');
        coap_blockwise_put_bytes(&resp, &slicer, entry_name, entry_len);
        if (is_dir) {
            coap_blockwise_put_char(&resp, &slicer, '/');
        }
        coap_blockwise_put_char(&resp, &slicer, '>');
    }

    vfs_closedir(&dir);
    coap_block2_finish(&slicer);

    return coap_builder_msg_size(&resp);
}

#if IS_USED(MODULE_NANOCOAP_FILESERVER_PUT)
static ssize_t _put_directory(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                              struct requestdata *request)
{
    vfs_DIR dir;
    uint8_t code = COAP_CODE_CHANGED;
    if (vfs_opendir(&dir, request->namebuf) == 0) {
        vfs_closedir(&dir);
        if (request->options.exists.if_match && request->options.if_match_len) {
            return _error_handler(pdu, buf, len, COAP_CODE_PRECONDITION_FAILED);
        }
    }
    else {
        if (request->options.exists.if_match) {
            return _error_handler(pdu, buf, len, COAP_CODE_PRECONDITION_FAILED);
        }
        int err;
        if ((err = vfs_mkdir(request->namebuf, 0777)) < 0) {
            return _error_handler(pdu, buf, len, err);
        }
        code = COAP_CODE_CREATED;
    }

    coap_builder_t resp;
    int err = coap_builder_init_reply(&resp, buf, len, pdu, code);
    if (err) {
        return err;
    }
    return coap_builder_msg_size(&resp);
}
#endif

#if IS_USED(MODULE_NANOCOAP_FILESERVER_DELETE)
static ssize_t _delete_directory(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                 struct requestdata *request)
{
    int err;
    if (request->options.exists.if_match && request->options.if_match_len) {
        if (request->options.if_match != byteorder_htonl(COAPFILESERVER_DIR_DELETE_ETAG).u32) {
            return _error_handler(pdu, buf, len, COAP_CODE_PRECONDITION_FAILED);
        }
        if ((err = vfs_rmdir(request->namebuf)) < 0) {
            return _error_handler(pdu, buf, len, err);
        }
    }
    else if (IS_USED(MODULE_VFS_UTIL)) {
        if ((err = vfs_unlink_recursive(request->namebuf,
                                        request->namebuf,
                                        sizeof(request->namebuf))) < 0) {
            return _error_handler(pdu, buf, len, err);
        }
    }

    coap_builder_t resp;
    err = coap_builder_init_reply(&resp, buf, len, pdu, COAP_CODE_DELETED);
    if (err) {
        return err;
    }
    return coap_builder_msg_size(&resp);
}
#endif

static ssize_t nanocoap_fileserver_directory_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                  struct requestdata *request,
                                                  const char *root, const char* resource_dir)
{
    switch (coap_get_method(pdu)) {
        case COAP_METHOD_GET:
            return _get_directory(pdu, buf, len, request, root, resource_dir);
#if IS_USED(MODULE_NANOCOAP_FILESERVER_PUT)
        case COAP_METHOD_PUT:
            return _put_directory(pdu, buf, len, request);
#endif
#if IS_USED(MODULE_NANOCOAP_FILESERVER_DELETE)
        case COAP_METHOD_DELETE:
            return _delete_directory(pdu, buf, len, request);
#endif
        default:
            return _error_handler(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
    }
}

ssize_t nanocoap_fileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                 coap_request_ctx_t *ctx) {
    const char *root = coap_request_ctx_get_context(ctx);
    const char *resource = coap_request_ctx_get_path(ctx);
    struct requestdata request = {0};

    /** Index in request.namebuf. Must not point at the last entry as that will be
     * zeroed to get a 0-terminated string. */
    size_t namelength = 0;
    uint8_t errorcode = COAP_CODE_INTERNAL_SERVER_ERROR;
    uint8_t strip_remaining = _count_char(resource, '/');

    /* If a root directory for the server was specified, use that */
    if (root && strlen(root) > 1) {
        strncpy(request.namebuf, root, sizeof(request.namebuf));
        namelength = strlen(root);
    }
    bool is_directory = true; /* either no path component at all or trailing '/' */
    {
        coap_optpos_t opt = { .offset = coap_get_total_hdr_len(pdu) };
        uint8_t *value;
        ssize_t optlen;
        while ((optlen = coap_opt_get_next(pdu, &opt, &value, 0)) != -ENOENT) {
            if (optlen < 0) {
                errorcode = COAP_CODE_BAD_REQUEST;
                goto error;
            }
            switch (opt.opt_num) {
                case COAP_OPT_IF_MATCH:
                    /* support only one if-match option, although it is repeatable */
                    if (request.options.exists.if_match) {
                        errorcode = COAP_CODE_NOT_IMPLEMENTED;
                        goto error;
                    }
                    if (optlen > 8) {
                        errorcode = COAP_CODE_BAD_OPTION;
                        goto error;
                    }
                    if (optlen && optlen != sizeof(request.options.if_match)) {
                        errorcode = COAP_CODE_PRECONDITION_FAILED;
                        goto error;
                    }
                    memcpy(&request.options.if_match, value,
                           request.options.if_match_len = optlen);
                    request.options.exists.if_match = true;
                    break;
                case COAP_OPT_IF_NONE_MATCH:
                    if (optlen || request.options.exists.if_none_match) {
                        errorcode = COAP_CODE_BAD_OPTION;
                        goto error;
                    }
                    request.options.exists.if_none_match = true;
                    break;
                case COAP_OPT_URI_HOST:
                    /* ignore host name option */
                    continue;
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
                    if (optlen != sizeof(request.options.etag)) {
                        /* Can't be a matching tag, no use in carrying that */
                        continue;
                    }
                    if (request.options.exists.etag) {
                        /* We can reasonably only check for a limited sized set,
                        * and it size is 1 here (sending multiple ETags is
                        * possible but rare) */
                        continue;
                    }
                    request.options.exists.etag = true;
                    memcpy(&request.options.etag, value, sizeof(request.options.etag));
                    break;
                case COAP_OPT_BLOCK2:
                    if (optlen > 4 || request.options.exists.block2) {
                        errorcode = COAP_CODE_BAD_OPTION;
                        goto error;
                    }
                    request.options.exists.block2 = true;
                    break;
                case COAP_OPT_BLOCK1:
                    if (optlen > 4 || request.options.exists.block1) {
                        errorcode = COAP_CODE_BAD_OPTION;
                        goto error;
                    }
                    request.options.exists.block1 = true;
                    break;
                case COAP_OPT_SIZE2:
                    request.options.exists.size2 = true;
                    break;
                default:
                    if (opt.opt_num & 1) {
                        errorcode = COAP_CODE_BAD_OPTION;
                        goto error;
                    }
                    break;
            }
        }
        if (request.options.exists.if_match &&
            request.options.exists.if_none_match) {
            errorcode = COAP_CODE_PRECONDITION_FAILED;
            goto error;
        }
    }
    request.namebuf[namelength] = '\0';

    DEBUG("request: '%s'\n", request.namebuf);

    /* Note to self: As we parse more options than just Uri-Path, we'll likely
     * pass a struct pointer later. So far, those could even be hooked into the
     * resource list, but that'll go away once we parse more options */
    return is_directory
        ? nanocoap_fileserver_directory_handler(pdu, buf, len, &request, root, resource)
        : nanocoap_fileserver_file_handler(pdu, buf, len, &request);
error:
    {
        coap_builder_t resp;
        int err = coap_builder_init_reply(&resp, buf, len, pdu, errorcode);
        if (err) {
            return err;
        }

        return coap_builder_msg_size(&resp);
    }
}

#ifdef MODULE_NANOCOAP_FILESERVER_CALLBACK
void nanocoap_fileserver_set_event_cb(nanocoap_fileserver_event_handler_t cb, void *ctx)
{
    mutex_lock(&_event_mtx);

    _event_cb = cb;
    _event_ctx = ctx;

    mutex_unlock(&_event_mtx);
}
#endif

/** @} */
