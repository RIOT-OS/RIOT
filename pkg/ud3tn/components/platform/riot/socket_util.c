// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#include "platform/hal_io.h"

#include "ud3tn/common.h"

#include <errno.h>
#include "net/af.h"
#include <stdio.h>

int create_unix_domain_socket(const char *path)
{
    // Unix domain socket is not supported on stm32 board
    /*int sock = socket(AF_UNIX, SOCK_STREAM, 0);

       if (sock == -1) {
        LOG_ERRNO("Socket Util", "socket(AF_UNIX)", errno);
        return -1;
       }

       struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
       };
       const int rv1 = snprintf(
        addr.sun_path,
        sizeof(addr.sun_path),
        "%s",
        path
       );

       if (rv1 <= 0 || (unsigned int)rv1 > sizeof(addr.sun_path)) {
        LOGF_ERROR(
            "Socket Util: Invalid socket path, len = %d, maxlen = %zu",
            rv1,
            sizeof(addr.sun_path)
        );
        return -1;
       }

       unlink(path);
       int rv2 = bind(
        sock,
        (const struct sockaddr *)&addr,
        sizeof(struct sockaddr_un)
       );
       if (rv2 == -1) {
        LOG_ERRNO("Socket Util", "bind(unix_domain_socket)", errno);
        return -1;
       }

       return sock;*/
    return 0;
}

int poll_recv_timeout(const int socket_fd, const int timeout)
{
    (void)socket_fd;
    (void)timeout;
    // POSIX select() is not available on RIOT bare-metal targets.
    // Use sock_tcp_read with timeout directly (aap2_peek_with_timeout).
    UD3TN_LOG_ERROR("Socket Util: poll_recv_timeout() not supported on RIOT!");
    return -1;
}
