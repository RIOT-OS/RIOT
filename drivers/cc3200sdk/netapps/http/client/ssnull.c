#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ti/net/network.h>

#include <ssock.h>

static ssize_t cprecv(void * ctx, int s, void * buf, size_t len, int flags);
static ssize_t cpencrypt(const void * ctx, uint8_t * cbuf, const uint8_t * buf, size_t len);
static void cpdelete(void * ctx);

Ssock_SecureFxns SSNullFxns = {
    cpencrypt,
    NULL,
    NULL,
    cprecv,
    cpdelete,
    0
};

void * SSNull_create(const uint8_t * ekey, const uint8_t * dkey)
{
    return (void *)1;
}

static void cpdelete(void * ctx)
{
}

static ssize_t cprecv(void * ctx, int s, void * buf, size_t len, int flags)
{
    return recv(s, buf, len, 0);
}

static ssize_t cpencrypt(const void * ctx, uint8_t * cbuf, const uint8_t * buf, size_t len)
{
    memcpy(cbuf, buf, len);

    return (len);
}
