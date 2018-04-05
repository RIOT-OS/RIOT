#include "net/af.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/tcp.h"

int tcp_send(gnrc_tcp_tcb_t *tcp, char *buffer, int buffer_size)
{
    ssize_t res, res_;

    for (res = 0; res < buffer_size; res += res_) {
        switch ((res_ =  gnrc_tcp_send(tcp, buffer + res, buffer_size - res, 0))) {
            case -ENOTCONN:
                printf("unable to send: ENOTCONN\n");
                gnrc_tcp_close(tcp);
                return 1;
            case -ECONNABORTED:
                printf("unable to send: ECONNABORTED\n");
                gnrc_tcp_close(tcp);
                return 1;
            case -ETIMEDOUT:
                printf("unable to send: ETIMEDOUT\n");
                gnrc_tcp_close(tcp);
                return 1;
            case -ECONNRESET:
                printf("unable to send: ECONNRESET\n");
                gnrc_tcp_close(tcp);
                return 1;
            default:
                if (res_ < 0) {
                    printf("unable to send: ?\n");
                    gnrc_tcp_close(tcp);
                    return 1;
                }
        }
    }
    return 0;
}

enum operation_t { GET, PUT, POST };
int ipfs_tcp(enum operation_t operation, char *full_url, uint16_t target_port, int donotread, char *data)
{
#define BUFFER_SIZE 256
    char *server, *url;
    char buffer[BUFFER_SIZE];
    int buffer_size;
    ssize_t res, res_;
    int ipfs_tcp_status;
    char *s, *ss;

    if (0 != strncmp(full_url, "ipfs_tcp://", 7)) {
        puts("invalid url");
        return 1;
    }
    if (NULL == (url = strchr(full_url + 7, '/'))) {
        puts("invalid url");
        return 1;
    }
    url[0] = '\0';
    url++;
    server = full_url + 7;
    printf("%s %s\n", server, url);

    /* connection */
    gnrc_tcp_tcb_t tcp;
    ipv6_addr_t target_addr;
    ipv6_addr_from_str(&target_addr, server);

#ifdef ipfs_tcpDEBUG
    puts("init");
#endif
    gnrc_tcp_init();
    gnrc_tcp_tcb_init(&tcp);
#ifdef ipfs_tcpDEBUG
    puts("init done");
#endif

    printf("connnect %s %d\n", server, target_port);
    switch (gnrc_tcp_open_active(&tcp, AF_INET6, (uint8_t *) &target_addr, target_port, 0)) {
        case 0:
            break;
        case -EISCONN:
            printf("unable to connect: EISCONN\n");
            return 1;
        case -EINVAL:
            printf("unable to connect: EINVAL\n");
            return 1;
        case -EAFNOSUPPORT:
            printf("unable to connect: EAFNOSUPPORT\n");
            return 1;
        case -EADDRINUSE:
            printf("unable to connect: EADDRINUSE\n");
            return 1;
        case -ECONNREFUSED:
            printf("unable to connect: ECINNREFUSED\n");
            return 1;
        case -ENOMEM:
            printf("unable to connect: ENOMEM\n");
            return 1;
        case -ETIMEDOUT:
            printf("unable to connect: ETIMEDOUT\n");
            return 1;
        default:
            printf("unable to connect: ?\n");
            return 1;
    }
#ifdef ipfs_tcpDEBUG
    puts("connected");
#endif

#ifdef ipfs_tcpDEBUG
    puts("send");
#endif
    buffer_size = snprintf(buffer, BUFFER_SIZE, "%s /%s ipfs_tcp/1.0\r\n", (GET == operation) ? "GET" : (PUT == operation) ? "PUT" : (POST == operation) ? "POST" : "HEAD", url);
    if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
        return res;
    }
    if (data != NULL) {
        buffer_size = snprintf(buffer, BUFFER_SIZE, "Content-Type: multipart/form-data; boundary=xxx\r\n");
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
        buffer_size = snprintf(buffer, BUFFER_SIZE, "Content-Length: %d\r\n\r\n", strlen(data) + 1 + 7 + 59 + 42 + 11);
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }

        /* request body */
        buffer_size = snprintf(buffer, BUFFER_SIZE, "--xxx\r\n"); // 7
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
        buffer_size = snprintf(buffer, BUFFER_SIZE, "Content-Disposition: form-data; name=\"file\"; filename=\"f\"\r\n"); //59
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
        buffer_size = snprintf(buffer, BUFFER_SIZE, "Content-Type: application/octet-stream\r\n\r\n"); // 42
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
        buffer_size = snprintf(buffer, BUFFER_SIZE, "%s\n", data);
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
        buffer_size = snprintf(buffer, BUFFER_SIZE, "\r\n--xxx--\r\n"); // 11
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
    }
    else {
        buffer_size = snprintf(buffer, BUFFER_SIZE, "\r\n");
        if (0 != (res = tcp_send(&tcp, buffer, buffer_size))) {
            return res;
        }
    }

#ifdef ipfs_tcpDEBUG
    puts("sent");
#endif

    if (1 == donotread) {
        gnrc_tcp_abort(&tcp);
        puts("closed");
        return 0;
    }

    res = 0;
    ipfs_tcp_status = -1;
    do {
#ifdef ipfs_tcpDEBUG
        puts("read header");
#endif
        res_ = gnrc_tcp_recv(&tcp, buffer, BUFFER_SIZE, GNRC_TCP_CONNECTION_TIMEOUT_DURATION);
        switch (res_) {
            case -ENOTCONN:
                printf("unable to receive: ENOTCONN\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -EAGAIN:
                printf("unable to receive: EAGAIN\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -ECONNABORTED:
                printf("unable to receive: ECONNABORTED\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -ETIMEDOUT:
                printf("unable to receive: ETIMEDOUT\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -ECONNRESET:
                printf("unable to receive: ECONNRESET\n");
                gnrc_tcp_close(&tcp);
                return 1;
            default:
                if (res_ < 0) {
                    printf("unable to receive: ?\n");
                    gnrc_tcp_close(&tcp);
                    return 1;
                }
        }
        buffer[res_] = '\0';
        printf("%s", buffer);
        fflush(stdout);
        /* ipfs_tcp status */
        if (-1 == ipfs_tcp_status && (NULL != (s = strstr(buffer, "ipfs_tcp/1.1")))) {
            s += 9;
            if (NULL != (ss = strstr(s, " "))) {
                *ss = '\0';
            }
            ipfs_tcp_status = atoi(s);
            if (NULL != ss) {
                *ss = ' ';
            }
        }
        /* content-length */
        if (NULL != (s = strstr(buffer, "Content-Length:"))) {
            s += 16;
            if (NULL != (ss = strstr(s, "\r\n"))) {
                *ss = '\0';
            }
            buffer_size = (ssize_t) atoi(s);
            if (NULL != ss) {
                *ss = '\r';
            }
        }
    } while (NULL == (s = strstr(buffer, "\r\n\r\n")));
#ifdef ipfs_tcpDEBUG
    puts("read header ok");
#endif
    s += 4;
    for (res = res_ - (s - buffer); res < buffer_size; res += res_) {
#ifdef ipfs_tcpDEBUG
        puts("read body");
#endif
        res_ = gnrc_tcp_recv(&tcp, buffer, ((buffer_size - res) > BUFFER_SIZE) ? BUFFER_SIZE : (buffer_size - res), GNRC_TCP_CONNECTION_TIMEOUT_DURATION);
        switch (res_) {
            case -ENOTCONN:
                printf("unable to receive: ENOTCONN\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -EAGAIN:
                printf("unable to receive: EAGAIN\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -ECONNABORTED:
                printf("unable to receive: ECONNABORTED\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -ETIMEDOUT:
                printf("unable to receive: ETIMEDOUT\n");
                gnrc_tcp_close(&tcp);
                return 1;
            case -ECONNRESET:
                printf("unable to receive: ECONNRESET\n");
                gnrc_tcp_close(&tcp);
                return 1;
            default:
                if (res_ < 0) {
                    printf("unable to receive: ?\n");
                    gnrc_tcp_close(&tcp);
                    return 1;
                }
        }
        buffer[res_] = '\0';
        printf("%s", buffer);
    }
#ifdef ipfs_tcpDEBUG
    puts("read body ok");
    puts("close");
#endif

#ifdef DOESNOTWORK
    gnrc_tcp_close(&tcp);
    puts("closed");
#endif
    gnrc_tcp_abort(&tcp);
    puts("closed");
    printf("Exited with ipfs_tcp status %d\n", ipfs_tcp_status);

    return 0;
}

int ipfs_tcp_cmd(int argc, char **argv)
{
    printf("%d\n", argc);
    enum operation_t operation;
    uint16_t target_port;
    int donotread = 0;
    if (argc <= 2) {
        printf("ipfs_tcp <get|put|post> <url> <port> <donotread> <data>\n");
        return 1;
    }
    if (argc >= 5 && 0 == strcmp(argv[4], "1")) {
        donotread = 1;
    }

    if (0 == strcmp(argv[1], "GET") || 0 == strcmp(argv[1], "get")) {
        operation = GET;
    }
    else if (0 == strcmp(argv[1], "PUT") || 0 == strcmp(argv[1], "put")) {
        operation = PUT;
    }
    else if (0 == strcmp(argv[1], "POST") || 0 == strcmp(argv[1], "post")) {
        operation = POST;
    }
    else {
        printf("unknown operation: \"%s\"\nipfs_tcp <get|put|post> <url> <port> <donotread> <data>\n", argv[1]);  return 1;
    }

    if (argc >= 4) {
        target_port = atoi(argv[3]);
    }
    else {
        target_port = 5001;
    }
    return ipfs_tcp(operation, argv[2], target_port, donotread, (argc >= 6) ? argv[5] : NULL);
}

