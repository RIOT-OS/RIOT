#include <stdio.h>

#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "msg.h"
#include "thread.h"

#include "server/keys.h"

#ifdef DTLS_ECC
#include "server/client_pub_keys.h"
#endif /* DTLS_ECC */

/* TinyDTLS WARNING check */
#ifdef WITH_RIOT_SOCKETS
#error TinyDTLS is set to use sockets but the app is configured for socks.
#endif

#define DTLS_STOP_SERVER_MSG 0x4001 /* Custom IPC type msg. */
#define DTLS_SERVER_PORT (20220)
#define READER_QUEUE_SIZE (8U)
#define MAX_SESSIONS    (5U)

char _dtls_server_stack[THREAD_STACKSIZE_MAIN +
                        THREAD_EXTRA_STACKSIZE_PRINTF];

static kernel_pid_t _dtls_server_pid = KERNEL_PID_UNDEF;

#define READER_QUEUE_SIZE (8U)

/* this credential management is too implementation specific, should be
 * should be improved later on */
#ifdef DTLS_PSK
static uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static int _key_storage(sock_dtls_t *sock, sock_dtls_session_t *session,
                        const unsigned char *id, size_t id_len,
                        unsigned char *key, size_t key_len)
{
    (void)sock;
    (void)session;
    (void)id;
    (void)id_len;
    puts("Server key storage");
    if (key_len < sizeof(psk_key_0) - 1) {
        return 0;
    }
    else {
        memcpy(key, psk_key_0, sizeof(psk_key_0) - 1);
        return sizeof(psk_key_0) - 1;
    }
}
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static dtls_ecdsa_key_t cert = {
    .priv_key = ecdsa_priv_key,
    .pub_key_x = ecdsa_pub_key_x,
    .pub_key_y = ecdsa_pub_key_y,
    .curve = DTLS_ECDH_CURVE_SECP256R1
};

static int _ecdsa_storage(sock_dtls_t *sock, sock_dtls_session_t *session,
                          dtls_ecdsa_key_t **key)
{
    (void)sock;
    (void)session;
    *key = &cert;
    return 0;
}

static int _ecdsa_verify(sock_dtls_t *sock, sock_dtls_session_t *session,
                         const unsigned char *pub_x, const unsigned char *pub_y,
                         size_t key_size)
{
    (void)sock;
    (void)session;
    if (!key_size) {
        return -1;
    }
    /* just checking every byte for now */
    for (unsigned i = 0; i < key_size; i++) {
        if ((pub_x[i] != client_ecdsa_pub_key_x[i]) ||
            (pub_y[i] != client_ecdsa_pub_key_y[i])) {
            puts("Client keys do not match");
            return -1;
        }
    }
    return 0;
}

#endif /* DLTS_ECC */

void *_dtls_server_wrapper(void *arg)
{
    (void) arg;

    ssize_t res;
    bool active = true;
    msg_t _reader_queue[READER_QUEUE_SIZE];
    msg_t msg;
    uint8_t rcv[512];

    /* Prepare (thread) messages reception */
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);

    sock_dtls_t sock = {
#ifdef DTLS_PSK
        .psk = {
            .psk_key_storage = _key_storage
        },
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
        .ecdsa = {
            .ecdsa_storage = _ecdsa_storage,
            .ecdsa_verify = _ecdsa_verify
        }
#endif /* DTLS_ECC */
    };

    sock_udp_t udp_sock;
    sock_udp_ep_t local_ep = SOCK_IPV6_EP_ANY;
    local_ep.port = DTLS_SERVER_PORT;
    sock_udp_create(&udp_sock, &local_ep, NULL, 0);

    sock_dtls_queue_t queue;
    sock_dtls_session_t queue_array[MAX_SESSIONS];
    sock_dtls_session_t rcv_session;

    sock_dtls_create(&sock, &udp_sock, 0);
    sock_dtls_init_server(&sock, &queue, queue_array, MAX_SESSIONS);

    while (active) {
        msg_try_receive(&msg);
        if (msg.type == DTLS_STOP_SERVER_MSG) {
            active = false;
        }
        else {
            res = sock_dtls_recv(&sock, &rcv_session, rcv, sizeof(rcv), SOCK_NO_TIMEOUT);
            if (res < 0) {
                printf("Error receiving UDP over DTLS %d", res);
                continue;
            }
            printf("Received %d bytes of DTLS message: %.*s\n", res, res, rcv);

            puts("Resending received message");
            res = sock_dtls_send(&sock, &rcv_session, rcv, res);
            if (res < 0) {
                printf("Error resending DTLS message: %d", res);
            }
        }
    }

    sock_dtls_destroy(&sock);
    puts("Terminating");
    return 0;
}

static void start_server(void)
{
    /* Only one instance of the server */
    if (_dtls_server_pid != KERNEL_PID_UNDEF) {
        puts("Error: server already running");
        return;
    }

    /* Start the server thread */
    _dtls_server_pid = thread_create(_dtls_server_stack,
                                     sizeof(_dtls_server_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     _dtls_server_wrapper, NULL, "dtls_server");

    /* Uncommon but better be sure */
    if (_dtls_server_pid == EINVAL) {
        puts("ERROR: Thread invalid");
        _dtls_server_pid = KERNEL_PID_UNDEF;
        return;
    }

    if (_dtls_server_pid == EOVERFLOW) {
        puts("ERROR: Thread overflow!");
        _dtls_server_pid = KERNEL_PID_UNDEF;
        return;
    }

    return;
}

static void stop_server(void)
{
    /* check if server is running at all */
    if (_dtls_server_pid == KERNEL_PID_UNDEF) {
        puts("Error: DTLS server is not running");
        return;
    }

    /* prepare the stop message */
    msg_t m;
    m.type = DTLS_STOP_SERVER_MSG;

    puts("Stopping server...");

    /* send the stop message to thread AND wait for (any) answer */
    msg_send_receive(&m, &m, _dtls_server_pid);

    _dtls_server_pid = KERNEL_PID_UNDEF;
    puts("Success: DTLS server stopped");
}

int dtls_server_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s start | stop\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "start") == 0) {
        start_server();
    }
    else if (strcmp(argv[1], "stop") == 0) {
        stop_server();
    }
    else {
        printf("Error: invalid command. Usage: %s start | stop\n", argv[0]);
    }
    return 0;
}
