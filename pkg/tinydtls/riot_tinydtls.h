#ifndef RIOT_TINYDTLS_H
#define RIOT_TINYDTLS_H

static int send_to_peer(struct dtls_context_t *ctx, session_t *session,
                        uint8_t *data, size_t len)
{
    int fd = *(int *)dtls_get_app_data(ctx);
    return sendto(fd, data, len, 0, &session->addr.sa, session->size);
}

#endif /* RIOT_TINYDTLS_H */
