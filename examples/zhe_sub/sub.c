#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>
#include "zhe-util.h"
#include "zhe-platform.h"

static int decvle14(const uint8_t *src, size_t sz, size_t *val)
{
    if (sz < 1) {
        return -1;
    } else if (src[0] <= 0x7f) {
        *val = src[0];
        return 1;
    } else if (sz == 1) {
        return 0;
    } else if (src[1] <= 0x7f) {
        *val = (src[1] << 7) | (src[0] & 0x7f);
        return 2;
    } else {
        return -1;
    }
}

static size_t isprint_runlen(const uint8_t *s, size_t n)
{
    size_t m;
    for (m = 0; m < n && s[m] != '"' && isprint(s[m]); m++)
        ;
    return m;
}

static void printraw(const uint8_t *src, size_t n)
{
    const char *sep = "";
    size_t i = 0, j;
    while (i < n) {
        size_t m = isprint_runlen(src + i, n - i);
        if (m >= 4) {
            printf("%s\"", sep);
            for (j = 0; j < m; j++) {
                printf("%c", (char)src[i + j]);
            }
            printf("\"");
            i += m;
        } else {
            printf("%s%d", sep, (int)src[i]);
            i++;
        }
        sep = ",";
    }
}

static void data_handler(zhe_rid_t rid, const void *payload, zhe_paysize_t size, void *vpub)
{
    size_t len;
    int lenlen;
    if ((lenlen = decvle14(payload, size, &len)) <= 0) {
        printraw(payload, size);
        printf("\n");
    } else {
        printf("%*.*s\n", (int)len, (int)len, (const char *)payload + lenlen);
    }
}

int main(int argc, char* argv[])
{
	argc=0;
	argv=NULL;
    struct zhe_platform *platform;
    zhe_subidx_t s;
    if (argc > 2) {
        fprintf(stderr, "usage: %s [BROKER-IP:PORT]\n", argv[0]);
        return 1;
    }
    platform = zhe(argc == 2 ? 0 : 7447, argc == 2 ? argv[1] : NULL);
    s = zhe_subscribe(1, 0, 0, data_handler, NULL);
    while (true) {
        zhe_dispatch(platform);
    }
}
