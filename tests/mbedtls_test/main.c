/*
 * Copyright (C) 2021_ML!PA Consulting GmbH
 */
/**
 * @ingroup     test app lwip_mbedtls
 * @{
 *
 * @file
 * @brief       send encrypted data to the server
 *
 * @}
 * @ author Mariem Charrada <mariem.charrada@ml-pa.com>
 */

#include "net/sock/tcp.h"
#include "shell.h"
#include "msg.h"
#include "xtimer.h"

#include "lwip/debug.h"
#include "mbedtls/debug.h"
#include "net_sockets.h"
#include "ssl.h"
#include "entropy.h"
#include "ctr_drbg.h"
#include "x509_crt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define MAIN_QUEUE_SIZE (4)

static void _lwip_mbedtls_my_debug(void *ctx, int level, const char *file, int line,
                                   const char *str)
{
    ((void)level);
    fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *)ctx);
}

static const unsigned char ca_certificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDnzCCAocCFGfDEoYXsh2tZvNCX+1ad8gBpKxKMA0GCSqGSIb3DQEBCwUAMIGL\n"
    "MQswCQYDVQQGEwJERTEPMA0GA1UECAwGQmVybGluMQ8wDQYDVQQHDAZCZXJsaW4x\n"
    "EDAOBgNVBAoMB2V4YW1wbGUxEDAOBgNVBAsMB2V4YW1wbGUxFDASBgNVBAMMC2V4\n"
    "YW1wbGUuY29tMSAwHgYJKoZIhvcNAQkBFhFleGFtcGxlQGdtYWlsLmNvbTAeFw0y\n"
    "MjAyMjAxMzMyMTdaFw0yMzAyMjAxMzMyMTdaMIGLMQswCQYDVQQGEwJERTEPMA0G\n"
    "A1UECAwGQmVybGluMQ8wDQYDVQQHDAZCZXJsaW4xEDAOBgNVBAoMB2V4YW1wbGUx\n"
    "EDAOBgNVBAsMB2V4YW1wbGUxFDASBgNVBAMMC2V4YW1wbGUuY29tMSAwHgYJKoZI\n"
    "hvcNAQkBFhFleGFtcGxlQGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEP\n"
    "ADCCAQoCggEBAKqWlRGRmVaA10tx8OrB6W/MHK02HyPh8Nl9KO9fst1FxD6pXVjt\n"
    "zwZcWrZoBGdqc2SdoUnkpVCm2vdSqp1rf9S+QFM3u+1fyhHCzNfkGWSojLuWOtb3\n"
    "szGpmq4sG8NU7FEf38k0H+3YrZ6RYrQQ5c/63BMRnqMlKNDW/bX6j///x4srdm/K\n"
    "2NXypqgR7nQsyq4+As+EJQDIsXY6vhFRtncMK9jki/tZXcPoY63vKqCoiU/+Z4lR\n"
    "cZeizmECF5uBNwWmwt4u0p7dCrN28oxWoDaQ5TYN9erPC1s+idHWqMJeUpmTf4p1\n"
    "q1X4YjD1Zrgbje6SYpRGYNSkgNVgi1TSJOUCAwEAATANBgkqhkiG9w0BAQsFAAOC\n"
    "AQEAVh6VaSNU+u9dDWhrOh8slzFGZCqkUI5kdngMPGFCLKgKFWfvSt6B7p9VppAt\n"
    "6yQiFUZG1ih6TNEM2t4kvIDNRTcfxoaDO5K1wrHD/v6z1aJllb1L4qcBJWa9XZXK\n"
    "8HlvguG6QU+RV34V/Bg52ToOr6dOopeT0sFRyzbOZcirlOFbkLc7e5etk8ZxfbOO\n"
    "l+cR01gyZCHEJiCdnLr8ZcijSIleNym6TiHtJXDXwyBEfUKQH1e9zeG9cH8R9kGD\n"
    "J0dlpVwmUT3L3p7rNopId1y9mZpctquXBvo0F1g/bycR5XLEN4LhGTnDgmcz6JdJ\n"
    "bKvRR2ATdCr4hgjJFtDMw2qzIw==\n"
    "-----END CERTIFICATE-----\n";

static int _lwip_mbedtls_client_thread(char *argv1, char *argv2, char *argv3)
{
    int status;
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt x509_certificate;
    static uint8_t client_connected = 0;

    if (!client_connected) {
        /* trusted chain configuration */
        mbedtls_x509_crt_init(&x509_certificate);
        DEBUG_PUTS("_lwip_mbedtls_client_thread: CA certificate init succeeded");
        status = mbedtls_x509_crt_parse(&x509_certificate, ca_certificate, sizeof(ca_certificate));
        if (status != 0) {
            DEBUG("_lwip_mbedtls_client_thread: CA certificate parse failed -0x%d\n", status);
            return -1;
        }
        DEBUG_PUTS("_lwip_mbedtls_client_thread: CA certificate parse succeeded");

        /* entropy source and PRNG */
        mbedtls_entropy_init(&entropy);
        DEBUG_PUTS("_lwip_mbedtls_client_thread: entropy init succeeded");
        mbedtls_ctr_drbg_init(&ctr_drbg);
        DEBUG_PUTS("_lwip_mbedtls_client_thread: random number generation init succeeded");
        status = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
        if (status != 0) {
            DEBUG("_lwip_mbedtls_client_thread: random number generation seed failed -0x%d\n",
                  status);
            return -1;
        }
        DEBUG_PUTS("_lwip_mbedtls_client_thread: random number generation seed succeeded");

        /* TLS configuration */
        mbedtls_ssl_config_init(&conf);
        DEBUG_PUTS("_lwip_mbedtls_client_thread: ssl config init succeeded");
        status = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                             MBEDTLS_SSL_TRANSPORT_STREAM,
                                             MBEDTLS_SSL_PRESET_DEFAULT);
        if (status != 0) {
            DEBUG("_lwip_mbedtls_client_thread: default ssl configuration failed -0x%d\n", status);
            return -1;
        }
        DEBUG_PUTS("_lwip_mbedtls_client_thread: default ssl configuration succeeded");

        // all cipher suits will be used

        //load CA certificate
        mbedtls_ssl_conf_ca_chain(&conf, &x509_certificate, NULL);

        //a must: ensure that certificates are signed by the CA
        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);

        //set the random engine
        mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

        //add debug output
        mbedtls_ssl_conf_dbg(&conf, _lwip_mbedtls_my_debug, stdout);
        mbedtls_debug_set_threshold(1);

        /* TLS context */
        mbedtls_ssl_init(&ssl);
        DEBUG_PUTS("_lwip_mbedtls_client_thread: ssl init succeeded");
        mbedtls_ssl_setup(&ssl, &conf);
        DEBUG_PUTS("_lwip_mbedtls_client_thread: ssl setup succeeded");

        /* Establish secure TLS connection */
        mbedtls_net_init(&server_fd);
        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
        status = mbedtls_net_connect(&server_fd, argv1, argv2, MBEDTLS_NET_PROTO_TCP);
        if (status != 0) {
            DEBUG("_lwip_mbedtls_client_thread: server connect failed -0x%d\n", status);
            return -1;
        }
        DEBUG_PUTS("_lwip_mbedtls_client_thread: server connect succeeded");
        client_connected = 1;
    }

    //verify certificate belongs to the server
    status = mbedtls_ssl_set_hostname(&ssl, "example.com");
    if (status != 0) {
        DEBUG("_lwip_mbedtls_client_thread: host verification failed -0x%d\n", status);
        return -1;
    }
    DEBUG_PUTS("_lwip_mbedtls_client_thread: ssl set hostname succeeded");
    status = mbedtls_ssl_handshake(&ssl);
    if (status != 0) {
        DEBUG("_lwip_mbedtls_client_thread: ssl handshake failed -0x%d\n", status);
        return -1;
    }
    DEBUG_PUTS("_lwip_mbedtls_client_thread: ssl handshake succeeded");
    status = mbedtls_ssl_get_verify_result(&ssl);
    if (status != 0) {
        DEBUG("_lwip_mbedtls_client_thread: certificate verification failed -0x%d\n", status);
        return -1;
    }
    DEBUG_PUTS("_lwip_mbedtls_client_thread: ssl get verify result succeeded");
    size_t write_buf_len = strlen(argv3);

    do {
        status = mbedtls_ssl_write(&ssl, (const unsigned char *)(argv3) + status,
                                   write_buf_len - status);
        if (status == 0) {
            DEBUG("_lwip_mbedtls_client_thread: %d bytes sent to server\n", write_buf_len);
            break;
        }
        else if (status < 0) {
            switch (status) {
            case MBEDTLS_ERR_SSL_WANT_READ:
            case MBEDTLS_ERR_SSL_WANT_WRITE:
            case MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS:
            case MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS:
            {
                continue;
            }
            default:
                DEBUG(
                    "_lwip_mbedtls_client_thread: default case in write ssl should never be reached\n");
            }
        }
    } while (1);

    DEBUG("_lwip_mbedtls_client_thread: sending encrypted %s to server is DONE :) \n", argv3);
    return 0;
}

static int cmd_tls_connect(int argc, char **argv)
{
    DEBUG_PUTS("main(): cmd_tls_connect started");
    if (argc < 3) {
        printf("usage: %s <host> <port> [data]\n", argv[0]);
        return -1;
    }

    return _lwip_mbedtls_client_thread(argv[1], argv[2], argv[3]);
}

static const shell_command_t shell_commands[] = {
    { "connect", "Perform a TLS connection", cmd_tls_connect },
    { NULL, NULL, NULL }
};

int main(void)
{
    msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    DEBUG_PUTS("main(): tls client starting");

    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    /* should be never reached */

    return 0;
}
