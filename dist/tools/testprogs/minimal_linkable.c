#include <reent.h>

int main(void)
{
    return 0;
}

void _exit(int n)
{
    (void)n;
    while(1);
}

void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
    (void)r;
    (void)incr;
    return NULL;
}

int _close_r(struct _reent *r, int fd)
{
    (void)r;
    (void)fd;
    return 0;
}

_off_t _lseek_r(struct _reent *r, int fd, _off_t off, int whence)
{
    (void)r;
    (void)fd;
    (void)off;
    (void)whence;
    return 0;
}

_ssize_t _read_r(struct _reent *r, int fd, void *buffer, size_t count)
{
    (void)r;
    (void)fd;
    (void)buffer;
    (void)count;

    return 0;
}

_ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t count)
{
    (void)r;
    (void)fd;
    (void)data;
    (void)count;

    return 0;
}
