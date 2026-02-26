// gnrc_wolfssl_tls.c

#include "gnrc_wolfssl_tls.h"
#include "wolfssl/ssl.h"
#include <net/sock/tcp.h>
#include <net/dns.h>
#include "net/ipv6/addr.h"
// #include <net/sock/tcp.h>
#include "ztimer.h"
#include <stdio.h>

struct TLSContext {
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;
    sock_tcp_t *tcp_socket;
    int timeout_ms_init;    // initial timeout
    int timeout_ms;         // current timeout
    const char *ca_cert_path;
    const char *device_cert_chain_path;
    const char *device_cert_path;
    const char *private_key_path;
};

static TLSContext tlsContext;
static sock_tcp_t internal_socket;

void (*log_callback)(const char *message);
void default_log_callback(const char *message)
{
    // puts(" [TLS] : ");
    puts(message);
    // puts("\n");
}
static int initialized = 0, isTLSConneted = 0;

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int tcp_init(void);
int tcp_connect(char *remoteAddress, int port, sock_tcp_t *socket);
int tcp_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int tcp_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);

int load_cert_from_path(TLSContext *context);
unsigned char *read_file_to_buffer(const char *filename, size_t *buffer_size);

int tls_init(TLSContext *context)
{
    if (initialized) {
        tls_log("TLS module already initialized");
        return 0;
    }

    int res = 0;

    if ((res = tcp_init()) < 0) {
        tls_log("tcp init failed");
        return res;
    }
    // Perform initialization steps if needed
    wolfSSL_Init();
    if ((context->ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        tls_log("ERROR: failed to create WOLFSSL_CTX\n");
        return -1;
    }
    wolfSSL_CTX_set_verify(context->ctx, SSL_VERIFY_NONE, 0);
    if (load_cert_from_path(context) < 0) {
        tls_log("failed to load certificate paths");
        return -1;
    }
    /* Set custom I/O callbacks and context */
    wolfSSL_CTX_SetIORecv(context->ctx, tcp_recv);
    wolfSSL_CTX_SetIOSend(context->ctx, tcp_send);
    tls_log("[TLS] module initialized successfully");

    initialized = 1;
    return 0;
}
int load_cert_from_path(TLSContext *context)
{
    unsigned char *ca_cert_buf;
    size_t ca_cert_buf_len;

    if (context->ca_cert_path != NULL) {
        ca_cert_buf = read_file_to_buffer(context->ca_cert_path, &ca_cert_buf_len);
        if (ca_cert_buf == NULL) {
            tls_log("failed to load ca cert from path");
            return -1;
        }
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_load_verify_buffer(context->ctx, ca_cert_buf,
                                           ca_cert_buf_len,
                                           WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            tls_log("ERROR: failed to load ca buffer\n");
            return -1;
        }
    }
    if (context->device_cert_chain_path != NULL) {
        ca_cert_buf = read_file_to_buffer(context->device_cert_chain_path, &ca_cert_buf_len);
        if (ca_cert_buf == NULL) {
            tls_log("failed to load device cert chain from path");
            return -1;
        }
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_certificate_chain_buffer_format(context->ctx, ca_cert_buf,
                                                            ca_cert_buf_len,
                                                            WOLFSSL_FILETYPE_PEM) !=
            WOLFSSL_SUCCESS) {
            tls_log("ERROR: failed to load device cert chain buffer\n");
            return -1;
        }
    }
    if (context->private_key_path != NULL) {
        ca_cert_buf = read_file_to_buffer(context->private_key_path, &ca_cert_buf_len);
        if (ca_cert_buf == NULL) {
            tls_log("failed to load private key from path");
            return -1;
        }
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_PrivateKey_buffer(context->ctx, ca_cert_buf,
                                              ca_cert_buf_len,
                                              WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            tls_log("ERROR: failed to load private key buffer\n");
            return -1;
        }
    }
    return 0;
}

void tls_log(const char *format, ...)
{
    char buffer[100];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Call the log callback
    log_callback(buffer);
}

TLSContext *tls_create_context(void (*_log_callback)(const char *message))
{
    log_callback = _log_callback == NULL ? default_log_callback : _log_callback;

    // Initialize the context members
    tlsContext.ctx = NULL;
    tlsContext.ssl = NULL;
    tlsContext.device_cert_chain_path = NULL;
    tlsContext.device_cert_path = NULL;
    tlsContext.private_key_path = NULL;
    tlsContext.ca_cert_path = NULL;
    tlsContext.timeout_ms_init = gnrc_wolfssl_tls_max_timeout_ms; // ssl handshake needs initial timeout
    tlsContext.timeout_ms = 0;
    tlsContext.tcp_socket = &internal_socket;
    return &tlsContext;
}

int tls_set_certificate(TLSContext *ctx, const char *ca_cert_path, const char *cert_chain_path,
                        const char *private_key_path)
{
    if (ctx == NULL) {
        tls_log("Invalid TLS context");
        return -1;
    }

    ctx->ca_cert_path = ca_cert_path;
    ctx->device_cert_chain_path = cert_chain_path;
    ctx->private_key_path = private_key_path;
    return 0;
}
int tls_create_session(TLSContext *context)
{
    if ((context->ssl = wolfSSL_new(context->ctx)) == NULL) {
        return -1;
    }
    wolfSSL_SetIOReadCtx(context->ssl, context);
    wolfSSL_SetIOWriteCtx(context->ssl, context);
    return 0;
}
int tls_establish_connection(char *remoteAddress, int port, TLSContext *context)
{
    if (context == NULL) {
        tls_log("Invalid TLS context");
        return -1;
    }
    int res;

    if ((res = tcp_connect(remoteAddress, port, context->tcp_socket)) < 0) {
        tls_log("tcp_connect failed");
        return res;
    }
    // Perform TLS connection setup using WolfSSL
    if ((res = tls_create_session(context)) < 0) {
        tls_log("ssl session creation failed");
        return res;
    }
    res = wolfSSL_connect(context->ssl);
    if (res != SSL_SUCCESS) {
        // Handle other SSL errors
        char text[100];
        int errCode = wolfSSL_get_error(context->ssl, res);
        wolfSSL_ERR_error_string(errCode, text);
        tls_log("SSL handshake failed: %d with %s\n", errCode, text);
        tls_close(context);
        return res;
    }
    isTLSConneted = 1;
    return 0;
}

int tls_send(TLSContext *context, unsigned char *buffer, int buffer_len, int timeout_ms)
{
    if (!isTLSConneted) {
        tls_log("TLS is not connected");
        return -1;
    }
    if (context == NULL) {
        tls_log("Invalid TLS context");
        return -1;
    }
    // Perform TLS data receiving using WolfSSL
    if (timeout_ms < 0) {
        tls_log("Negative timeout value");
        return -1;
    }
    else {
        timeout_ms = timeout_ms <
                     gnrc_wolfssl_tls_max_timeout_ms ? timeout_ms : gnrc_wolfssl_tls_max_timeout_ms;
    }

    ztimer_now_t end_time = ztimer_now(ZTIMER_MSEC) + timeout_ms;
    int sentLen = 0, chunk_size, res;

    while ((sentLen < buffer_len) && (ztimer_now(ZTIMER_MSEC) < end_time)) {
        chunk_size = buffer_len - sentLen;
        res = wolfSSL_write(context->ssl, (unsigned char *)buffer + sentLen, chunk_size);
        if (res > 0) {
            sentLen += res;
        }
        else if (res <= 0) {
            sentLen = res;
            int err = -wolfSSL_get_error(context->ssl, res);
            switch (err) {
            case WOLFSSL_CBIO_ERR_GENERAL:
            case WOLFSSL_CBIO_ERR_CONN_CLOSE:
                tls_close(context);
                return err;
            default:
                break;
            }
        }
    }
    return sentLen;
}

int tls_receive(TLSContext *context, unsigned char *buffer, int buffer_len, int timeout_ms)
{
    if (!isTLSConneted) {
        tls_log("TLS is not connected");
        return -1;
    }
    if (context == NULL) {
        tls_log("Invalid TLS context");
        return -1;
    }
    if (buffer_len <= 0) {
        return 0;
    }
    // Perform TLS data receiving using WolfSSL
    if (timeout_ms < 0) {
        tls_log("Negative timeout value");
        return -1;
    }
    else if (timeout_ms == 0) {
        context->timeout_ms = context->timeout_ms_init = 0;
    }
    else {
        context->timeout_ms = timeout_ms <
                              gnrc_wolfssl_tls_max_timeout_ms ? timeout_ms :
                              gnrc_wolfssl_tls_max_timeout_ms;
    }
    int res = wolfSSL_read(context->ssl, buffer, buffer_len);

    if (res > 0) {
        return res;
    }
    // Handle other SSL errors
    int err = -wolfSSL_get_error(context->ssl, res); // SOCKET_PEER_CLOSED_E

    switch (err) {
    case WOLFSSL_CBIO_ERR_GENERAL:
    case WOLFSSL_CBIO_ERR_CONN_CLOSE:
        tls_log("WOLFSSL_CBIO_ERR_CONN_CLOSE from TCP_READ");
        tls_close(context);
        break;
    case WOLFSSL_CBIO_ERR_WANT_READ:
        return 0;
    default:
        break;
    }
    return err;
}

void tls_close(TLSContext *context)
{
    if (!isTLSConneted) {
        tls_log("TLS is not connected so no need for tls_close");
        return;
    }
    if (context == NULL) {
        tls_log("Invalid TLS context");
        return;
    }
    isTLSConneted = 0;
    // Perform TLS connection closing using WolfSSL
    if (context->tcp_socket) {
        sock_tcp_disconnect(context->tcp_socket);
        tls_log("TLS module cleaned up sock");
    }

    // Free allocated resources

    if (context->ssl) {
        wolfSSL_free(context->ssl);
    }
    if (context->ctx) {
        wolfSSL_CTX_free(context->ctx);
    }
    wolfSSL_Cleanup();
    tls_log("TLS module cleaned up context");
    initialized = 0;
    tls_log("TLS module cleaned up");
}

/* Custom I/O send function */
int tcp_send(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    if (ctx == NULL) {
        tls_log("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    // casting to get IOReadCtx
    TLSContext *context = (TLSContext *)ctx;

    if (!context) {
        tls_log(
            "underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    ssize_t bytesSent = sock_tcp_write(context->tcp_socket, buf, sz);

    if (bytesSent >= 0) {
        if (bytesSent == 0) {
            return WOLFSSL_CBIO_ERR_WANT_WRITE;
        }
    }
    else {
        switch (bytesSent) {
        case -ENOTCONN:
            // printf("TCP_SEND conn close...\n");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        case -EAGAIN:
            return 0;
        default:
            tls_log("TCP Error while sending data: %zd\n", bytesSent);
            break;
        }
    }

    return bytesSent;
}

int tcp_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    if (ctx == NULL) {
        tls_log("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    TLSContext *context = (TLSContext *)ctx;

    if (!context) {
        tls_log(
            "underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    // Use sock_tcp_read to receive data from the socket
    int timeout = context->timeout_ms == 0 ? context->timeout_ms_init : context->timeout_ms;

    int recvLen = 0;
    ztimer_now_t endTimestamp = ztimer_now(ZTIMER_MSEC) + timeout;
    ssize_t bytesRead;

    do{
        bytesRead = sock_tcp_read(context->tcp_socket, buf + recvLen, sz - recvLen, 0);
        if (bytesRead > 0) {
            recvLen += bytesRead;
        }

    } while ((recvLen < sz) && (ztimer_now(ZTIMER_MSEC) < endTimestamp));
    if (recvLen > 0) {
        // tls_log("Received %zd (total size %d) bytes: %s\n", recvLen, sz, buf);
        return recvLen;
    }
    else {
        switch (bytesRead) {
        case -ETIMEDOUT:
            //printf("Reading timeout...\n");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        case -EAGAIN:
            return WOLFSSL_CBIO_ERR_WANT_READ;
        case 0:
            tls_log("Connection closed by the peer.\n");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;

        default:
            tls_log("Error while receiving data: %zd\n", bytesRead);
            break;
        }
    }

    return bytesRead;
}

int tcp_connect(char *remoteAddress, int port, sock_tcp_t *socket)
{
    int res;
    sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;

    remote.port = port;

    // TODO : remoteIP has to retrieved from remoteAddress
    // DNS64 is not currently configured
    int dps = 1;
    char *remoteIP = dps ? "64:ff9b::3374:91ca" : "64:ff9b::2871:b0b5";

    ////////////////////////////////
    // if ((res=sock_dns_query(remoteAddress, (ipv6_addr_t *)&remote.addr, AF_INET6)) < 0) {
    //     printf("Error resolving hostname %d\n",res);
    //     return -1;
    // }
    // //////////////////////////////

    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, remoteIP);

    if ((res = sock_tcp_connect(socket, &remote, 0, 0)) < 0) {
        tls_log("Error connecting sock %s\n", strerror(res));
    }
    return res;
}

int tcp_init(void)
{
    // if any initialization needed in future
    return 0;
}

unsigned char *read_file_to_buffer(const char *filename, size_t *buffer_size)
{
    FILE *file = fopen(filename, "rb"); // Open the file in binary read mode

    if (!file) {
        tls_log("Failed to open file");
        return NULL;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    unsigned char *buffer = (unsigned char *)malloc(file_size);

    if (!buffer) {
        tls_log("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);

    fclose(file);

    if (bytes_read != (size_t)file_size) {
        tls_log("Failed to read file");
        free(buffer);
        return NULL;
    }

    if (buffer_size) {
        *buffer_size = (size_t)file_size;
    }

    return buffer;
}
