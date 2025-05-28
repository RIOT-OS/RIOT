#include <stdio.h>
#include "net/sock/tls.h"
#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "msg.h"
#include "net/sock/tcp.h"
#include "net/gnrc.h"
#include "net/ipv6/addr.h"
#include <wolfssl/ssl.h>
#include "ztimer.h"

int main(void)
{
    printf("Testing TLS module\n");

    /* Create a TLS socket */
    sock_tls_tcp_t sock;
    int ret = sock_tls_tcp_create(&sock, wolfTLSv1_2_client_method());

    if (ret == 0) {
        printf("TLS socket created successfully\n");
    } else {
        printf("Failed to create TLS socket: %d\n", ret);
    }

    return 0;
}
