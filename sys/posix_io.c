#include <msg.h>

#include <posix_io.h>


static int _posix_fileop(int pid, int op, int flags) {
    msg_t m;
    m.type = op;
    m.content.value = flags;
    msg_send_receive(&m, &m, pid);
    return m.content.value;
}

static int _posix_fileop_data(int pid, int op, char* buffer, int nbytes) {
    struct posix_iop_t r;
    r.nbytes = nbytes;
    r.buffer = buffer;

    msg_t m;
    m.type = op;
    m.content.ptr = (char*) &r;

    msg_send_receive(&m, &m, pid);

    return r.nbytes;
}

int posix_open(int pid, int flags) {
    return _posix_fileop(pid, OPEN, flags); 
}

int posix_close(int pid) {
    return _posix_fileop(pid, CLOSE, 0); 
}

int posix_read(int pid, char* buffer, int bufsize) {
    return _posix_fileop_data(pid, READ, buffer, bufsize);
}

int posix_write(int pid, char* buffer, int bufsize) {
    return _posix_fileop_data(pid, WRITE, buffer, bufsize);
}
