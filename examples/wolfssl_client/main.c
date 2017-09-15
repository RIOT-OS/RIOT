/*
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA 
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       wolfSSL client example
 *
 * @author      Kaleb J. Himes <kaleb@wolfssl.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* wolfSSL */
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>

#define DEFAULT_PORT 11111

int main(void)
{
    int                sockfd;
    struct sockaddr_in servAddr;
    char               buff[22] = "Hello wolfSSL Server!\0";
    char               server_ip[10] = "127.0.0.1\0";
    size_t             len;

    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx;
    WOLFSSL*     ssl;

/*----------------------------------------------------------------------------*/
/* TLS Setup:
 * This section will need resolved on a per-device basis depending on the
 * available TCP/IP stack
 */
/*----------------------------------------------------------------------------*/

    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        exit(-1);
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family = AF_INET;             /* using IPv4      */
    servAddr.sin_port   = htons(DEFAULT_PORT); /* on DEFAULT_PORT */

    /* Get the server IPv4 address from the command line call */
    if (inet_pton(AF_INET, server_ip, &servAddr.sin_addr) != 1) {
        fprintf(stderr, "ERROR: invalid address\n");
        exit(-1);
    }

    /* Connect to the server */
    if (connect(sockfd, (struct sockaddr*) &servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "ERROR: failed to connect\n");
        exit(-1);
    }
/*----------------------------------------------------------------------------*/
/* END TCP SETUP, BEGIN TLS */
/*----------------------------------------------------------------------------*/

    /* Initialize wolfSSL */
    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        exit(-1);
    }

    /* Load client certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
                                       sizeof_ca_cert_der_2048,
                                       SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load ca buffer\n");
        exit(-1);
    }

    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        exit(-1);
    }

    /* Attach wolfSSL to the socket */
    wolfSSL_set_fd(ssl, sockfd);

    /* Connect to wolfSSL on the server side */
    if (wolfSSL_connect(ssl) != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to connect to wolfSSL\n");
        exit(-1);
    }

    /* Get a message for the server from stdin */
    printf("Message for server: %s\n", buff);
    len = strnlen(buff, sizeof(buff));

    /* Send the message to the server */
    if (wolfSSL_write(ssl, buff, len) != (int) len) {
        fprintf(stderr, "ERROR: failed to write\n");
        exit(-1);
    }

    /* Read the server data into our buff array */
    memset(buff, 0, sizeof(buff));
    if (wolfSSL_read(ssl, buff, sizeof(buff)-1) == -1) {
        fprintf(stderr, "ERROR: failed to read\n");
        exit(-1);
    }

    /* Print to stdout any data the server sends */
    printf("Server sent a reply!\n");
    printf("Server Response was:  %s\n", buff);

    /* Cleanup and exit */
    wolfSSL_free(ssl);      /* Free the wolfSSL object                  */
    wolfSSL_CTX_free(ctx);  /* Free the wolfSSL context object          */
    wolfSSL_Cleanup();      /* Cleanup the wolfSSL environment          */
    close(sockfd);          /* Close the connection to the server       */

    exit(0);               /* Return reporting a success               */
}
