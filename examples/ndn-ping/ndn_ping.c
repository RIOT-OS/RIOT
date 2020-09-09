/*
 * Copyright (C) 2016 Wentao Shang
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       NDN ping client and server implementation
 *
 * @author      Wentao Shang <wentaoshang@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "random.h"
#include "test_utils/expect.h"

#include "ndn-riot/app.h"
#include "ndn-riot/ndn.h"
#include "ndn-riot/encoding/name.h"
#include "ndn-riot/encoding/interest.h"
#include "ndn-riot/encoding/data.h"
#include "ndn-riot/msg-type.h"

static ndn_app_t* handle = NULL;

static const uint8_t ecc_key_pri[] = {
    0x38, 0x67, 0x54, 0x73, 0x8B, 0x72, 0x4C, 0xD6,
    0x3E, 0xBD, 0x52, 0xF3, 0x64, 0xD8, 0xF5, 0x7F,
    0xB5, 0xE6, 0xF2, 0x9F, 0xC2, 0x7B, 0xD6, 0x90,
    0x42, 0x9D, 0xC8, 0xCE, 0xF0, 0xDE, 0x75, 0xB3
};

static const uint8_t ecc_key_pub[] = {
    0x2C, 0x3C, 0x18, 0xCB, 0x31, 0x88, 0x0B, 0xC3,
    0x73, 0xF4, 0x4A, 0xD4, 0x3F, 0x8C, 0x80, 0x24,
    0xD4, 0x8E, 0xBE, 0xB4, 0xAD, 0xF0, 0x69, 0xA6,
    0xFE, 0x29, 0x12, 0xAC, 0xC1, 0xE1, 0x26, 0x7E,
    0x2B, 0x25, 0x69, 0x02, 0xD5, 0x85, 0x51, 0x4B,
    0x91, 0xAC, 0xB9, 0xD1, 0x19, 0xE9, 0x5E, 0x97,
    0x20, 0xBB, 0x16, 0x2A, 0xD3, 0x2F, 0xB5, 0x11,
    0x1B, 0xD1, 0xAF, 0x76, 0xDB, 0xAD, 0xB8, 0xCE
};

static int on_data(ndn_block_t* interest, ndn_block_t* data)
{
    (void)interest;

    ndn_block_t name;
    int r = ndn_data_get_name(data, &name);
    expect(r == 0);
    printf("client (pid=%" PRIkernel_pid "): data received, name=",
           handle->id);
    ndn_name_print(&name);
    putchar('\n');

    ndn_block_t content;
    r = ndn_data_get_content(data, &content);
    expect(r == 0);
    expect(content.len == 6);

    printf("client (pid=%" PRIkernel_pid "): content=%02X%02X%02X%02X\n",
           handle->id, *(content.buf + 2), *(content.buf + 3),
           *(content.buf + 4), *(content.buf + 5));

    r = ndn_data_verify_signature(data, ecc_key_pub, sizeof(ecc_key_pub));

    if (r != 0) {
      printf("client (pid=%" PRIkernel_pid "): fail to verify signature\n",
             handle->id);
    }
    else {
      printf("client (pid=%" PRIkernel_pid "): signature valid\n",
             handle->id);
    }

    return NDN_APP_CONTINUE;
}

static int on_timeout(ndn_block_t* interest)
{
    ndn_block_t name;
    int r = ndn_interest_get_name(interest, &name);
    expect(r == 0);

    printf("client (pid=%" PRIkernel_pid "): interest timeout, name=",
           handle->id);
    ndn_name_print(&name);
    putchar('\n');

    return NDN_APP_CONTINUE;
}

static uint16_t count = 0;
static uint16_t max_count;

static int send_interest(void* context)
{
    const char* uri = (const char*)context;

    printf("client (pid=%" PRIkernel_pid "): in sched callback, count=%d\n",
            handle->id, ++count);
    if (count > max_count) {
        /* This is pure hack: ideally should wait for all pending I/O requests
        * to finish before stopping the app.  However, this may cause the app
        * to block forever if not implemented very carefully. */
        printf("client (pid=%" PRIkernel_pid "): stop the app\n", handle->id);
        return NDN_APP_STOP;
    }

    ndn_shared_block_t* sn = ndn_name_from_uri(uri, strlen(uri));
    if (sn == NULL) {
        printf("client (pid=%" PRIkernel_pid "): cannot create name from uri "
               "\"%s\"\n", handle->id, uri);
        return NDN_APP_ERROR;
    }

    uint32_t rand = random_uint32();
    ndn_shared_block_t* sin = ndn_name_append_uint32(&sn->block, rand);
    ndn_shared_block_release(sn);
    if (sin == NULL) {
        printf("client (pid=%" PRIkernel_pid "): cannot append component to "
               "name \"%s\"\n", handle->id, uri);
        return NDN_APP_ERROR;
    }

    uint32_t lifetime = 1000;  // 1 sec

    printf("client (pid=%" PRIkernel_pid "): express interest, name=",
           handle->id);
    ndn_name_print(&sin->block);
    putchar('\n');

    if (ndn_app_express_interest(handle, &sin->block, NULL, lifetime,
                                 on_data, on_timeout) != 0) {
        printf("client (pid=%" PRIkernel_pid "): failed to express interest\n",
               handle->id);
        ndn_shared_block_release(sin);
        return NDN_APP_ERROR;
    }
    ndn_shared_block_release(sin);

    if (ndn_app_schedule(handle, send_interest, context, 2000000) != 0) {
        printf("client (pid=%" PRIkernel_pid "): cannot schedule next interest"
               "\n", handle->id);
        return NDN_APP_ERROR;
    }
    printf("client (pid=%" PRIkernel_pid "): schedule next interest in 2 sec"
           "\n", handle->id);

    return NDN_APP_CONTINUE;
}

static void run_client(const char* uri, int max_cnt)
{
    printf("client (pid=%" PRIkernel_pid "): start\n", thread_getpid());

    handle = ndn_app_create();
    if (handle == NULL) {
        printf("client (pid=%" PRIkernel_pid "): cannot create app handle\n",
               thread_getpid());
        return;
    }

    max_count = max_cnt;
    count = 0;

    if (ndn_app_schedule(handle, send_interest, (void*)uri, 1000000) != 0) {
        printf("client (pid=%" PRIkernel_pid "): cannot schedule first "
               "interest\n", handle->id);
        ndn_app_destroy(handle);
        return;
    }
    printf("client (pid=%" PRIkernel_pid "): schedule first interest in 1 sec"
           "\n", handle->id);

    printf("client (pid=%" PRIkernel_pid "): enter app run loop\n",
           handle->id);

    ndn_app_run(handle);

    printf("client (pid=%" PRIkernel_pid "): returned from app run loop\n",
           handle->id);

    ndn_app_destroy(handle);
}

static uint8_t sid = 0;

static int on_interest(ndn_block_t* interest)
{
    ndn_block_t in;
    if (ndn_interest_get_name(interest, &in) != 0) {
        printf("server (pid=%" PRIkernel_pid "): cannot get name from interest"
               "\n", handle->id);
        return NDN_APP_ERROR;
    }

    printf("server (pid=%" PRIkernel_pid "): interest received, name=",
           handle->id);
    ndn_name_print(&in);
    putchar('\n');

    ndn_shared_block_t* sdn = ndn_name_append_uint8(&in, sid);
    if (sdn == NULL) {
        printf("server (pid=%" PRIkernel_pid "): cannot append component to "
               "name\n", handle->id);
        return NDN_APP_ERROR;
    }

    ndn_metainfo_t meta = { NDN_CONTENT_TYPE_BLOB, -1 };

    uint32_t rand = random_uint32();
    uint8_t* buf = (uint8_t*)(&rand);
    ndn_block_t content = { buf, sizeof(rand) };

    ndn_shared_block_t* sd =
    ndn_data_create(&sdn->block, &meta, &content,
                    NDN_SIG_TYPE_ECDSA_SHA256, NULL,
                    ecc_key_pri, sizeof(ecc_key_pri));
    if (sd == NULL) {
        printf("server (pid=%" PRIkernel_pid "): cannot create data block\n",
               handle->id);
        ndn_shared_block_release(sdn);
        return NDN_APP_ERROR;
    }

    printf("server (pid=%" PRIkernel_pid "): send data to NDN thread, name=",
           handle->id);
    ndn_name_print(&sdn->block);
    putchar('\n');
    ndn_shared_block_release(sdn);

    /* pass ownership of "sd" to the API */
    if (ndn_app_put_data(handle, sd) != 0) {
        printf("server (pid=%" PRIkernel_pid "): cannot put data\n",
               handle->id);
        return NDN_APP_ERROR;
    }

    printf("server (pid=%" PRIkernel_pid "): return to the app\n", handle->id);
    return NDN_APP_CONTINUE;
}

static void run_server(const char* prefix, int id)
{
    printf("server (pid=%" PRIkernel_pid "): start\n", thread_getpid());

    handle = ndn_app_create();
    if (handle == NULL) {
        printf("server (pid=%" PRIkernel_pid "): cannot create app handle\n",
               thread_getpid());
        return;
    }
    sid = (uint8_t)id;

    ndn_shared_block_t* sp = ndn_name_from_uri(prefix, strlen(prefix));
    if (sp == NULL) {
        printf("server (pid=%" PRIkernel_pid "): cannot create name from uri "
               "\"%s\"\n", handle->id, prefix);
        return;
    }

    printf("server (pid=%" PRIkernel_pid "): register prefix \"%s\"\n",
           handle->id, prefix);
    /* pass ownership of "sp" to the API */
    if (ndn_app_register_prefix(handle, sp, on_interest) != 0) {
        printf("server (pid=%" PRIkernel_pid "): failed to register prefix\n",
               handle->id);
        ndn_app_destroy(handle);
        return;
    }

    printf("server (pid=%" PRIkernel_pid "): enter app run loop\n",
           handle->id);

    ndn_app_run(handle);

    printf("server (pid=%" PRIkernel_pid "): returned from app run loop\n",
           handle->id);

    ndn_app_destroy(handle);
}

int ndn_ping(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [client|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "client") == 0) {
        if (argc < 4) {
            printf("usage: %s client _name_uri_ _max_count_\n", argv[0]);
            return 1;
        }

        int max_cnt = atoi(argv[3]);
        if (max_cnt == 0) {
            printf("invalid max count number: %s\n", argv[3]);
            return 1;
        }

        run_client(argv[2], max_cnt);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 4) {
            printf("usage: %s server _prefix_ _server_id_\n", argv[0]);
            return 1;
        }

        run_server(argv[2], atoi(argv[3]));
    }
    else {
        puts("error: invalid command");
    }
    return 0;
}
