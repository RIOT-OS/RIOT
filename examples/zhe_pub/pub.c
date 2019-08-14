#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "zhe-util.h"
#include "zhe-platform.h"

int main(int argc, char *argv[])
{
	argc=0;
	argv=NULL;
    struct zhe_platform *platform;
    zhe_pubidx_t p;
    if (argc > 2) {
        fprintf(stderr, "usage: %s [BROKER-IP:PORT]\n", argv[0]);
        return 1;
    }
    platform = zhe(argc == 2 ? 0 : 7447, argc == 2 ? argv[1] : NULL);
    p = zhe_publish(1, 0, 1);
    uint64_t delay = 1000000000;
    zhe_once(platform, delay);
    uint32_t count = 0;
    while (true) {
        unsigned char buf[12];
        int sz = snprintf((char*)buf+1, sizeof(buf)-1, "%"PRIu32, count);
        buf[0] = (unsigned char)sz;
        printf(">> Writing %s\n", buf);
        zhe_write(p, buf, (zhe_paysize_t)(1+sz), zhe_platform_time());
        zhe_flush(zhe_platform_time());
        count += 1;
        zhe_once(platform, delay);
    }
}
