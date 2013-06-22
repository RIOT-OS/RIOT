#ifndef __READ_H
#define __READ_H

#define OPEN 0
#define CLOSE 1
#define READ 2
#define WRITE 3

struct posix_iop_t {
    int nbytes;
    char *buffer;
};

int posix_open(int pid, int flags);
int posix_close(int pid);
int posix_read(int pid, char *buffer, int bufsize);
int posix_write(int pid, char *buffer, int bufsize);

#endif /* __READ_H */
