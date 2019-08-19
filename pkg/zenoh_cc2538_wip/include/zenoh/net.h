#ifndef ZENOH_C_NET_H_
#define ZENOH_C_NET_H_

#include "zenoh/types.h"
#include "zenoh/codec.h"

Z_RESULT_DECLARE (z_socket_t, socket)

z_socket_result_t open_tx_session(const char *locator);

int z_send_buf(z_socket_t sock, z_iobuf_t* buf);

int z_recv_buf(z_socket_t sock, z_iobuf_t *buf);

int z_recv_n(z_socket_t sock, uint8_t* buf, size_t len);

size_t z_send_msg(z_socket_t sock, z_iobuf_t* buf, z_message_t* m);

size_t z_send_large_msg(z_socket_t sock, z_iobuf_t* buf, z_message_t* m, unsigned int max_len);

z_vle_result_t z_recv_vle(z_socket_t sock);

z_message_p_result_t z_recv_msg(z_socket_t sock, z_iobuf_t* buf);
void z_recv_msg_na(z_socket_t sock, z_iobuf_t* buf, z_message_p_result_t *r);

#endif /* ZENOH_C_NET_H_ */
