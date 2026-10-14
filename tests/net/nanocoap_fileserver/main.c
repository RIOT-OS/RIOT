/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests for the nanocoap fileserver
 *
 * Requests are assembled in memory and passed directly to
 * @ref nanocoap_fileserver_handler, the file system is the native one.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "net/nanocoap.h"
#include "net/nanocoap/fileserver.h"
#include "vfs.h"
#include "vfs_default.h"
#include "vfs_util.h"

#define TEST_ROOT       VFS_DEFAULT_DATA "/fstest"
#define BUF_SIZE        (256)
#define SMALL_BLK_SZX   (1) /* 32 bytes */

static const coap_resource_t _resource = {
    .path = "/fs",
    .methods = COAP_GET | COAP_PUT | COAP_DELETE | COAP_POST | COAP_MATCH_SUBTREE,
    .handler = nanocoap_fileserver_handler,
    .context = (void *)TEST_ROOT,
};

/** Optional request parameters */
typedef struct {
    const uint32_t *if_match;       /**< Etag for If-Match */
    const uint32_t *etag;           /**< Etag option */
    bool if_none_match;             /**< add If-None-Match */
    bool block1;                    /**< add Block1 option */
    bool block2;                    /**< add Block2 option */
    uint8_t szx;                    /**< block size for Block1 / Block2 */
    uint32_t num;                   /**< block number for Block1 / Block2 */
    bool more;                      /**< more flag for Block1 */
    const void *payload;            /**< request payload */
    size_t payload_len;             /**< length of the payload */
} req_t;

static uint8_t _resp_buf[BUF_SIZE];
static coap_pkt_t _resp;

/** Send a request to the file server, parse the response into _resp */
static int _request(unsigned method, const char *path, const req_t *req)
{
    uint8_t buf[BUF_SIZE];
    coap_pkt_t pkt;
    req_t none = {0};
    char uri[64];

    if (!req) {
        req = &none;
    }

    ssize_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_CON, NULL, 0, method, 1);
    coap_pkt_init(&pkt, buf, sizeof(buf), len);

    /* options in ascending order */
    if (req->if_match) {
        coap_opt_add_opaque(&pkt, COAP_OPT_IF_MATCH, req->if_match, sizeof(uint32_t));
    }
    if (req->etag) {
        coap_opt_add_opaque(&pkt, COAP_OPT_ETAG, req->etag, sizeof(uint32_t));
    }
    if (req->if_none_match) {
        coap_opt_add_opaque(&pkt, COAP_OPT_IF_NONE_MATCH, NULL, 0);
    }
    /* the fileserver strips the resource path, so prefix it */
    snprintf(uri, sizeof(uri), "%s%s", _resource.path, path);
    coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, uri, '/');
    if (req->block2) {
        coap_opt_add_uint(&pkt, COAP_OPT_BLOCK2, (req->num << 4) | req->szx);
    }
    if (req->block1) {
        coap_opt_add_uint(&pkt, COAP_OPT_BLOCK1,
                          (req->num << 4) | (req->more << 3) | req->szx);
    }
    if (req->payload_len) {
        len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);
        memcpy(pkt.payload, req->payload, req->payload_len);
        len += req->payload_len;
    }
    else {
        len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    }

    /* re-parse as the server would do */
    if (coap_parse(&pkt, buf, len) < 0) {
        return -EBADMSG;
    }

    coap_request_ctx_t ctx = { .resource = &_resource };
    ssize_t resp_len = nanocoap_fileserver_handler(&pkt, _resp_buf, sizeof(_resp_buf), &ctx);
    if (resp_len <= 0) {
        return -EIO;
    }
    if (coap_parse(&_resp, _resp_buf, resp_len) < 0) {
        return -EBADMSG;
    }

    return coap_get_code_raw(&_resp);
}

static int _put(const char *path, const void *data, size_t len, const req_t *opts)
{
    req_t req = opts ? *opts : (req_t){0};
    req.payload = data;
    req.payload_len = len;
    return _request(COAP_METHOD_PUT, path, &req);
}

static int _get(const char *path, const req_t *opts)
{
    return _request(COAP_METHOD_GET, path, opts);
}

static bool _payload_is(const char *s)
{
    return _resp.payload_len == strlen(s) && !memcmp(_resp.payload, s, _resp.payload_len);
}

static bool _payload_contains(const char *s)
{
    size_t len = strlen(s);
    for (size_t i = 0; i + len <= _resp.payload_len; ++i) {
        if (!memcmp(&_resp.payload[i], s, len)) {
            return true;
        }
    }
    return false;
}

static bool _get_etag(uint32_t *etag)
{
    uint8_t *val;
    if (coap_opt_get_opaque(&_resp, COAP_OPT_ETAG, &val) != sizeof(*etag)) {
        return false;
    }
    memcpy(etag, val, sizeof(*etag));
    return true;
}

static bool _file_exists(const char *path)
{
    struct stat st;
    char name[64];
    snprintf(name, sizeof(name), "%s%s", TEST_ROOT, path);
    return vfs_stat(name, &st) == 0;
}

static void _write_file(const char *path, const char *content)
{
    char name[64];
    snprintf(name, sizeof(name), "%s%s", TEST_ROOT, path);
    int fd = vfs_open(name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    TEST_ASSERT(fd >= 0);
    TEST_ASSERT_EQUAL_INT(strlen(content), vfs_write(fd, content, strlen(content)));
    vfs_close(fd);
}

static void _set_up(void)
{
    char path[64];
    strcpy(path, TEST_ROOT);
    vfs_unlink_recursive(TEST_ROOT, path, sizeof(path));
    TEST_ASSERT_EQUAL_INT(0, vfs_mkdir(TEST_ROOT, 0777));
}

static void _tear_down(void)
{
    char path[64];
    strcpy(path, TEST_ROOT);
    vfs_unlink_recursive(TEST_ROOT, path, sizeof(path));
}

static void test_get_file(void)
{
    uint32_t etag = 0;
    _write_file("/hello", "hello world");

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/hello", NULL));
    TEST_ASSERT(_payload_is("hello world"));
    TEST_ASSERT(_get_etag(&etag));
}

static void test_get_not_found(void)
{
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _get("/nope", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _get("/nope/nope", NULL));
}

static void test_path_traversal(void)
{
    _write_file("/secret", "x");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _get("/../fstest/secret", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _get("/./secret", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _put("/../x", "x", 1, NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _request(COAP_METHOD_DELETE, "/../fstest", NULL));
    TEST_ASSERT(_file_exists("/secret"));
}

static void test_path_too_long(void)
{
    char path[128];
    memset(path, 'a', sizeof(path));
    path[0] = '/';
    path[sizeof(path) - 1] = 0;
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _get(path, NULL));
}

static void test_get_blockwise(void)
{
    /* 32 byte blocks: "0123456789abcdef" * 5 = 80 bytes -> 3 blocks */
    const char *content = "0123456789abcdef0123456789abcdef0123456789abcdef"
                          "0123456789abcdef0123456789abcdef";
    _write_file("/big", content);

    for (unsigned i = 0; i < 3; ++i) {
        req_t req = { .block2 = true, .szx = SMALL_BLK_SZX, .num = i };
        coap_block1_t block;
        TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/big", &req));
        TEST_ASSERT(coap_get_block2(&_resp, &block));
        TEST_ASSERT_EQUAL_INT(i, block.blknum);
        TEST_ASSERT_EQUAL_INT(i < 2, block.more);
        TEST_ASSERT_EQUAL_INT(i < 2 ? 32 : 80 - 64, _resp.payload_len);
        TEST_ASSERT(!memcmp(_resp.payload, &content[i * 32], _resp.payload_len));
    }

    /* block past the end */
    req_t req = { .block2 = true, .szx = SMALL_BLK_SZX, .num = 10 };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/big", &req));
    TEST_ASSERT_EQUAL_INT(0, _resp.payload_len);
}

static void test_get_exact_block_size(void)
{
    /* file size is a multiple of the block size: last block must not claim more */
    _write_file("/exact", "0123456789abcdef0123456789abcdef");
    req_t req = { .block2 = true, .szx = SMALL_BLK_SZX, .num = 0 };
    coap_block1_t block;
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/exact", &req));
    TEST_ASSERT(coap_get_block2(&_resp, &block));
    TEST_ASSERT_EQUAL_INT(0, block.more);
    TEST_ASSERT_EQUAL_INT(32, _resp.payload_len);
}

static void test_get_empty_file(void)
{
    _write_file("/empty", "");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/empty", NULL));
    TEST_ASSERT_EQUAL_INT(0, _resp.payload_len);
}

static void test_etag(void)
{
    uint32_t etag, other;
    _write_file("/tagged", "content");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/tagged", NULL));
    TEST_ASSERT(_get_etag(&etag));

    /* matching Etag: not modified */
    req_t req = { .etag = &etag };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_VALID, _get("/tagged", &req));

    /* non-matching Etag: full response */
    other = ~etag;
    req.etag = &other;
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/tagged", &req));

    /* If-Match */
    req = (req_t){ .if_match = &etag };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/tagged", &req));
    req.if_match = &other;
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _get("/tagged", &req));
}

static void test_directory_listing(void)
{
    TEST_ASSERT_EQUAL_INT(0, vfs_mkdir(TEST_ROOT "/sub", 0777));
    _write_file("/file", "x");
    _write_file("/.hidden", "x");

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/", NULL));
    TEST_ASSERT(_payload_contains("</fs/file>"));
    TEST_ASSERT(_payload_contains("</fs/sub/>"));
    TEST_ASSERT(!_payload_contains("hidden"));
    TEST_ASSERT_EQUAL_INT(COAP_FORMAT_LINK, coap_get_content_type(&_resp));

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/sub/", NULL));
    TEST_ASSERT_EQUAL_INT(0, _resp.payload_len);

    _write_file("/sub/inner", "x");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/sub/", NULL));
    TEST_ASSERT(_payload_is("</fs/sub/inner>"));

    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _get("/nodir/", NULL));
}

static void test_directory_listing_blockwise(void)
{
    char name[16];
    char all[BUF_SIZE * 2] = "";

    for (unsigned i = 0; i < 10; ++i) {
        snprintf(name, sizeof(name), "/file%02u", i);
        _write_file(name, "x");
    }

    /* collect all blocks */
    for (unsigned i = 0; i < 10; ++i) {
        coap_block1_t block;
        req_t req = { .block2 = true, .szx = SMALL_BLK_SZX, .num = i };
        TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/", &req));
        TEST_ASSERT(coap_get_block2(&_resp, &block));
        strncat(all, (char *)_resp.payload, _resp.payload_len);
        if (!block.more) {
            break;
        }
    }

    for (unsigned i = 0; i < 10; ++i) {
        snprintf(name, sizeof(name), "</fs/file%02u>", i);
        TEST_ASSERT(strstr(all, name));
    }
}

static void test_directory_if_match(void)
{
    uint32_t etag = 1;
    req_t req = { .if_match = &etag };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _get("/", &req));
}

static void test_put_create(void)
{
    uint32_t etag, etag_get;
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CREATED, _put("/new", "data", 4, NULL));
    TEST_ASSERT(_get_etag(&etag));
    TEST_ASSERT(_file_exists("/new"));
    /* no temporary file left over */
    TEST_ASSERT(!_file_exists("/.new"));

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/new", NULL));
    TEST_ASSERT(_payload_is("data"));
    TEST_ASSERT(_get_etag(&etag_get));
    TEST_ASSERT_EQUAL_INT(etag, etag_get);
}

static void test_put_replace_truncates(void)
{
    _write_file("/file", "a long file content");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CHANGED, _put("/file", "short", 5, NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/file", NULL));
    TEST_ASSERT(_payload_is("short"));
}

static void test_put_preconditions(void)
{
    uint32_t etag, other;
    _write_file("/file", "old");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/file", NULL));
    TEST_ASSERT(_get_etag(&etag));
    other = ~etag;

    /* If-None-Match on existing file */
    req_t req = { .if_none_match = true };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _put("/file", "new", 3, &req));
    /* If-None-Match on new file */
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CREATED, _put("/file2", "new", 3, &req));

    /* If-Match with wrong Etag */
    req = (req_t){ .if_match = &other };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _put("/file", "new", 3, &req));
    /* If-Match on non-existing file */
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _put("/file3", "new", 3, &req));
    TEST_ASSERT(!_file_exists("/file3"));
    TEST_ASSERT(!_file_exists("/.file3"));

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/file", NULL));
    TEST_ASSERT(_payload_is("old"));

    /* If-Match with correct Etag */
    req = (req_t){ .if_match = &etag };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CHANGED, _put("/file", "new", 3, &req));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/file", NULL));
    TEST_ASSERT(_payload_is("new"));
}

static void test_put_blockwise_create(void)
{
    const char *content = "0123456789abcdef0123456789abcdef"
                          "0123456789abcdef0123456789abcdef"
                          "0123456789";

    for (unsigned i = 0; i < 3; ++i) {
        req_t req = { .block1 = true, .szx = SMALL_BLK_SZX, .num = i, .more = i < 2 };
        size_t len = (i < 2) ? 32 : 10;
        int res = _put("/blocks", &content[i * 32], len, &req);
        TEST_ASSERT_EQUAL_INT(i < 2 ? COAP_CODE_CONTINUE : COAP_CODE_CREATED, res);
        /* file only appears once complete */
        TEST_ASSERT_EQUAL_INT(i == 2, _file_exists("/blocks"));
    }

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/blocks", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/blocks", &(req_t){ .block2 = true, .szx = SMALL_BLK_SZX, .num = 2 }));
    TEST_ASSERT(_payload_is("0123456789"));
    TEST_ASSERT(!_file_exists("/.blocks"));
}

static void test_put_blockwise_out_of_order(void)
{
    const char *content = "0123456789abcdef0123456789abcdef";
    req_t req = { .block1 = true, .szx = SMALL_BLK_SZX, .num = 1, .more = true };

    /* initial block missing */
    TEST_ASSERT_EQUAL_INT(COAP_CODE_REQUEST_ENTITY_INCOMPLETE, _put("/gap", content, 32, &req));
    TEST_ASSERT(!_file_exists("/gap"));
    TEST_ASSERT(!_file_exists("/.gap"));
}

static void test_put_blockwise_duplicate(void)
{
    const char *content = "0123456789abcdef0123456789abcdef";
    req_t req = { .block1 = true, .szx = SMALL_BLK_SZX, .num = 0, .more = true };

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTINUE, _put("/dup", content, 32, &req));
    /* retransmission of the same block is accepted and must not corrupt the file */
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTINUE, _put("/dup", content, 32, &req));
    req = (req_t){ .block1 = true, .szx = SMALL_BLK_SZX, .num = 1, .more = false };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CREATED, _put("/dup", "end", 3, &req));

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/dup", &(req_t){ .block2 = true, .szx = SMALL_BLK_SZX, .num = 0 }));
    TEST_ASSERT_EQUAL_INT(32, _resp.payload_len);
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/dup", &(req_t){ .block2 = true, .szx = SMALL_BLK_SZX, .num = 1 }));
    TEST_ASSERT(_payload_is("end"));
}

static void test_put_name_length_limit(void)
{
    /* the temporary name needs one more character than the final name */
    char path[64];
    for (unsigned len = 20; len < sizeof(path) - 1; ++len) {
        memset(path, 'a', len);
        path[0] = '/';
        path[len] = 0;
        int res = _put(path, "x", 1, NULL);
        /* must either work or be rejected, not corrupt memory */
        TEST_ASSERT(res == COAP_CODE_CREATED || res >= COAP_CODE_PATH_NOT_FOUND);
        _request(COAP_METHOD_DELETE, path, NULL);
    }
}

static void test_delete_file(void)
{
    uint32_t etag, other;
    _write_file("/del", "x");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/del", NULL));
    TEST_ASSERT(_get_etag(&etag));
    other = ~etag;

    req_t req = { .if_match = &other };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _request(COAP_METHOD_DELETE, "/del", &req));
    TEST_ASSERT(_file_exists("/del"));

    req.if_match = &etag;
    TEST_ASSERT_EQUAL_INT(COAP_CODE_DELETED, _request(COAP_METHOD_DELETE, "/del", &req));
    TEST_ASSERT(!_file_exists("/del"));

    TEST_ASSERT_EQUAL_INT(COAP_CODE_PATH_NOT_FOUND, _request(COAP_METHOD_DELETE, "/del", NULL));
}

static void test_directories(void)
{
    /* create */
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CREATED, _request(COAP_METHOD_PUT, "/dir/", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CHANGED, _request(COAP_METHOD_PUT, "/dir/", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/dir/", NULL));

    /* If-Match on creation can never match */
    uint32_t etag = 1;
    req_t req = { .if_match = &etag };
    TEST_ASSERT_EQUAL_INT(COAP_CODE_PRECONDITION_FAILED, _request(COAP_METHOD_PUT, "/dir2/", &req));
    TEST_ASSERT(!_file_exists("/dir2"));

    /* recursive delete */
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CREATED, _put("/dir/file", "x", 1, NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_DELETED, _request(COAP_METHOD_DELETE, "/dir/", NULL));
    TEST_ASSERT(!_file_exists("/dir"));
}

static void test_delete_directory_etag(void)
{
    TEST_ASSERT_EQUAL_INT(0, vfs_mkdir(TEST_ROOT "/dir", 0777));
    _write_file("/dir/file", "x");

    /* the magic etag only removes empty directories */
    uint32_t etag = byteorder_htonl(COAPFILESERVER_DIR_DELETE_ETAG).u32;
    req_t req = { .if_match = &etag };
    TEST_ASSERT(_request(COAP_METHOD_DELETE, "/dir/", &req) != COAP_CODE_DELETED);
    TEST_ASSERT(_file_exists("/dir/file"));

    TEST_ASSERT_EQUAL_INT(0, vfs_unlink(TEST_ROOT "/dir/file"));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_DELETED, _request(COAP_METHOD_DELETE, "/dir/", &req));
    TEST_ASSERT(!_file_exists("/dir"));
}

static void test_method_not_allowed(void)
{
    _write_file("/file", "x");
    TEST_ASSERT_EQUAL_INT(COAP_CODE_METHOD_NOT_ALLOWED, _request(COAP_METHOD_POST, "/file", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_METHOD_NOT_ALLOWED, _request(COAP_METHOD_POST, "/", NULL));
}

static const char *_last_event_path;
static int _events[8];
static unsigned _num_events;

static void _event_cb(nanocoap_fileserver_event_t event, nanocoap_fileserver_event_ctx_t *ctx)
{
    (void)ctx;
    if (_num_events < ARRAY_SIZE(_events)) {
        _events[_num_events++] = event;
    }
    _last_event_path = ctx->path;
}

static void test_events(void)
{
    nanocoap_fileserver_set_event_cb(_event_cb, NULL);
    _num_events = 0;

    TEST_ASSERT_EQUAL_INT(COAP_CODE_CREATED, _put("/ev", "x", 1, NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, _get("/ev", NULL));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_DELETED, _request(COAP_METHOD_DELETE, "/ev", NULL));
    nanocoap_fileserver_set_event_cb(NULL, NULL);

    TEST_ASSERT_EQUAL_INT(5, _num_events);
    TEST_ASSERT_EQUAL_INT(NANOCOAP_FILESERVER_PUT_FILE_START, _events[0]);
    TEST_ASSERT_EQUAL_INT(NANOCOAP_FILESERVER_PUT_FILE_END, _events[1]);
    TEST_ASSERT_EQUAL_INT(NANOCOAP_FILESERVER_GET_FILE_START, _events[2]);
    TEST_ASSERT_EQUAL_INT(NANOCOAP_FILESERVER_GET_FILE_END, _events[3]);
    TEST_ASSERT_EQUAL_INT(NANOCOAP_FILESERVER_DELETE_FILE, _events[4]);
}

Test *tests_fileserver(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_get_file),
        new_TestFixture(test_get_not_found),
        new_TestFixture(test_path_traversal),
        new_TestFixture(test_path_too_long),
        new_TestFixture(test_get_blockwise),
        new_TestFixture(test_get_exact_block_size),
        new_TestFixture(test_get_empty_file),
        new_TestFixture(test_etag),
        new_TestFixture(test_directory_listing),
        new_TestFixture(test_directory_listing_blockwise),
        new_TestFixture(test_directory_if_match),
        new_TestFixture(test_put_create),
        new_TestFixture(test_put_replace_truncates),
        new_TestFixture(test_put_preconditions),
        new_TestFixture(test_put_blockwise_create),
        new_TestFixture(test_put_blockwise_out_of_order),
        new_TestFixture(test_put_blockwise_duplicate),
        new_TestFixture(test_put_name_length_limit),
        new_TestFixture(test_delete_file),
        new_TestFixture(test_directories),
        new_TestFixture(test_delete_directory_etag),
        new_TestFixture(test_method_not_allowed),
        new_TestFixture(test_events),
    };

    EMB_UNIT_TESTCALLER(fileserver_tests, _set_up, _tear_down, fixtures);
    return (Test *)&fileserver_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_fileserver());
    TESTS_END();

    return 0;
}
