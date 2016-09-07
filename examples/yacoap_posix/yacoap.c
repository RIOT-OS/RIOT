/*
 * Copyright (C) 2016 smlng <s@mlng.net>
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
 * @brief       YaCoAP handler functions and definition of CoAP resources
 *
 * @author      smlng <s@mlng.net>
 *
 * @}
 */

// standard
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// network
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
// riot
#include "thread.h"
#include "coap.h"

// parameters
#define COAP_BUF_SIZE           (255)
#define COAP_MSG_QUEUE_SIZE     (8U)
#define COAP_WKC_LENGTH         (256)
#define COAP_THREAD_STACKSIZE   (2 * THREAD_STACKSIZE_DEFAULT)

static char coap_thread_stack[COAP_THREAD_STACKSIZE];
static msg_t coap_thread_msg_queue[COAP_MSG_QUEUE_SIZE];
static char wkc[COAP_WKC_LENGTH];

static const coap_resource_path_t path_well_known_core = {2, {".well-known", "core"}};
static int handle_get_well_known_core(const coap_resource_t *resource,
                                      const coap_packet_t *inpkt,
                                      coap_packet_t *pkt)
{
    printf("handle_get_well_known_core\n");
    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              resource->msg_type, COAP_RSPCODE_CONTENT,
                              resource->content_type,
                              (const uint8_t *)wkc, strlen(wkc),
                              pkt);
}

static const coap_resource_path_t path_riot_board = {2, {"riot", "board"}};
static int handle_get_riot_board(const coap_resource_t *resource,
                                 const coap_packet_t *inpkt,
                                 coap_packet_t *pkt)
{
    printf("handle_get_riot_board\n");
    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              resource->msg_type, COAP_RSPCODE_CONTENT,
                              resource->content_type,
                              (const uint8_t *)RIOT_BOARD, strlen(RIOT_BOARD),
                              pkt);
}

static const coap_resource_path_t path_riot_mcu = {2, {"riot", "mcu"}};
static int handle_get_riot_mcu(const coap_resource_t *resource,
                               const coap_packet_t *inpkt,
                               coap_packet_t *pkt)
{
    printf("handle_get_riot_mcu\n");
    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              resource->msg_type, COAP_RSPCODE_CONTENT,
                              resource->content_type,
                              (const uint8_t *)RIOT_MCU, strlen(RIOT_MCU),
                              pkt);
}

// CoAP server resources
coap_resource_t resources[] =
{
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        handle_get_well_known_core, &path_well_known_core,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_APP_LINKFORMAT)},
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_NONCON,
        handle_get_riot_board, &path_riot_board,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)},
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_NONCON,
        handle_get_riot_mcu, &path_riot_mcu,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)},
    {(coap_state_t)0, (coap_method_t)0, (coap_msgtype_t)0,
        NULL, NULL,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)}
};

/**
 * @brief CoAP server thread handling incoming requests
 */
static void *coap_server(void *arg)
{
    (void) arg;
    // start coap listener
    struct sockaddr_in6 srvaddr;
    char cliaddr[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;
    int sock = -1;
    uint8_t buf[COAP_BUF_SIZE];

    msg_init_queue(coap_thread_msg_queue, COAP_MSG_QUEUE_SIZE);
    sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    /* parse port */
    port = (uint16_t)COAP_DEFAULT_PORT;
    srvaddr.sin6_family = AF_INET6;
    memset(&srvaddr.sin6_addr, 0, sizeof(srvaddr.sin6_addr));
    srvaddr.sin6_port = htons(port);
    if (sock < 0) {
        puts("ERROR: initializing socket");
        return NULL;
    }
    if (bind(sock, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) < 0) {
        puts("ERROR: binding socket");
        return NULL;
    }
    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(cliaddr);
        coap_packet_t pkt;

        n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
        printf("received message of %d bytes\n", n);

        if ((rc = coap_parse(buf, n, &pkt)) > COAP_ERR) {
            printf("Bad packet rc=%d\n", rc);
        }
        else {
            for (int state = COAP_RSP_WAIT; state != COAP_RSP_SEND; ) {
                size_t buflen = sizeof(buf);
                coap_packet_t rsppkt;
                state = coap_handle_request(resources, &pkt, &rsppkt);

                if ((rc = coap_build(&rsppkt, buf, &buflen)) > COAP_ERR) {
                    printf("coap_build failed rc=%d\n", rc);
                    break;
                }
                else {
                    printf("send response\n");
                    sendto(sock, buf, buflen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
                }
            }
        }
    }
    return NULL;
}

/**
 * @brief start CoAP server thread
 *
 * @return PID of coap thread
 */
int coap_start_thread(void)
{
    // init .well-known/core response based on coap resources
    if (coap_make_link_format(resources, wkc, COAP_WKC_LENGTH) < 0) {
        return -1;
    }
    // start thread
    return thread_create(coap_thread_stack, sizeof(coap_thread_stack),
                         THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,
                         coap_server, NULL, "coap_server");
}

/* ---------------------------- CoAP CLIENT PART ---------------------------- */
static int handle_request_get_response(const coap_resource_t *resource,
                                       const coap_packet_t *reqpkt,
                                       coap_packet_t *rsppkt)
{
    (void) resource;
    (void) reqpkt;
    printf("handle_get_well_known_core\n");
    printf("  content: %.*s\n", (int)rsppkt->payload.len, (char *)rsppkt->payload.p);
    return COAP_RSP_RECV;
}

static int handle_request_put_response(const coap_resource_t *resource,
                                       const coap_packet_t *reqpkt,
                                       coap_packet_t *rsppkt)
{
    (void) resource;
    (void) reqpkt;
    printf("handle_request_put_response\n");
    if (rsppkt->hdr.t == COAP_TYPE_ACK) {
        printf("  ACK\n");
        return COAP_ACK_RECV;
    }
    printf("  INVALID\n");
    return COAP_ACK_WAIT;
}

// CoAP client resources for requests
coap_resource_t requests[] =
{
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_CON,
        handle_request_get_response, &path_well_known_core,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_APP_LINKFORMAT)},
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_CON,
        handle_request_put_response, NULL,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)},
    {(coap_state_t)0, (coap_method_t)0, (coap_msgtype_t)0,
        NULL, NULL,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)}
};

/**
 * @brief Send CoAP GET request for .well-known/core
 * @note blocks until valid response
 *
 * @param ip CoAP server IP address string
 *
 * @return 0 on success, error code otherwise
 */
int coap_get(const char *ip)
{
    struct sockaddr_in6 dst, cliaddr;
    uint16_t port = COAP_DEFAULT_PORT;
    int fd;
    dst.sin6_family = AF_INET6;
    /* parse destination address */
    if (inet_pton(AF_INET6, ip, &dst.sin6_addr) != 1) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse port */
    dst.sin6_port = htons(port);
    fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        puts("error init send socket");
        return 1;
    }
    coap_packet_t req, rsp;
    int n, rc;
    uint16_t msgid = 42;
    printf("coap_make_request\n");
    coap_make_request(msgid, NULL, &requests[0], NULL, 0, &req);
    uint8_t buf[256];
    size_t buflen = sizeof(buf);
    if ((rc = coap_build(&req, buf, &buflen)) > COAP_ERR) {
        printf("coap_build failed rc=%d\n", rc);
        return 1;
    }
    else {
        printf("send get request\n");
        if ((n = sendto(fd, buf, buflen, 0, (struct sockaddr *)&dst, sizeof(dst))) == -1) {
            puts("error sendto");
            return 1;
        }
        printf("wait for response ...\n");
        for (int state = COAP_RSP_WAIT; state != COAP_RSP_RECV; ) {
            socklen_t len = sizeof(cliaddr);
            n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
            printf("received message of %d bytes\n", n);
            if ((rc = coap_parse(buf, n, &rsp)) > COAP_ERR) {
                printf("Bad packet rc=%d\n", rc);
                return 1;
            }
            state = coap_handle_response(requests, &req, &rsp);
        }
    }
    close(fd);
    return 0;
}

/**
 * @brief Send CoAP PUT request
 * @note blocks until valid response
 *
 * @param ip CoAP server IP address string
 * @param path Path of requested CoAP resource
 * @param content PUT request payload
 *
 * @return 0 on success, error code otherwise
 */
int coap_put(const char *ip, const char *path, const char *content)
{
    struct sockaddr_in6 dst, cliaddr;
    uint16_t port = COAP_DEFAULT_PORT;
    int fd;
    dst.sin6_family = AF_INET6;
    /* parse destination address */
    if (inet_pton(AF_INET6, ip, &dst.sin6_addr) != 1) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse port */
    dst.sin6_port = htons(port);
    fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        puts("error init send socket");
        return 1;
    }
    coap_resource_path_t path_request_put;
    path_request_put.count = 1;
    path_request_put.items[0] = path;
    for (size_t c = 0; c < strlen(path); ++c) {
        if (path[c] == '/')
            ++path_request_put.count;
        if (path_request_put.count > COAP_MAX_PATHITEMS) {
            fprintf(stderr, "path has too many elements: %d\n", path_request_put.count);
            return 1;
        }
        path_request_put.items[path_request_put.count] = &path[c+1];
    }
    requests[1].path = &path_request_put;
    coap_packet_t req, rsp;
    int n, rc;
    uint16_t msgid = 42;
    coap_make_request(msgid, NULL, &resources[0], (uint8_t *)content, strlen(content), &req);
    uint8_t buf[256];
    size_t buflen = sizeof(buf);
    if ((rc = coap_build(&req, buf, &buflen)) > COAP_ERR) {
        printf("coap_build failed rc=%d\n", rc);
        return 1;
    }
    else {
        printf("send put request\n");
        if ((n = sendto(fd, buf, buflen, 0, (struct sockaddr *)&dst, sizeof(dst))) == -1) {
            puts("error sendto");
            return 1;
        }
        printf("wait for response ...\n");
        for (int state = COAP_ACK_WAIT; state != COAP_ACK_RECV; ) {
            socklen_t len = sizeof(cliaddr);
            n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
            printf("received message of %d bytes\n", n);
            if ((rc = coap_parse(buf, n, &rsp)) > COAP_ERR) {
                printf("Bad packet rc=%d\n", rc);
                return 1;
            }
            state = coap_handle_response(resources, &req, &rsp);
        }
    }
    // cleanup and exit
    close(fd);
    return 0;
}
