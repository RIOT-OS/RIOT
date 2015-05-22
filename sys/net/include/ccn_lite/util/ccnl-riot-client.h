/*
 * @f ccnl-riot-client.h client side api for ccnl on RIOT
 *
 * Copyright (C) 2013, Christian Mehlis, Freie Universität Berlin
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @ingroup ccnl
 * @{
 * @file
 * @brief   CCN high level client functions
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @}
 */
#ifndef CCNL_RIOT_CLIENT_H
#define CCNL_RIOT_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  high level function to fetch a file (all chunks of a file)
 *
 * @param relay_pid pid of the relay thread
 *
 * @param name c string represenation of the name to fetch e.g. "/riot/test"
 *
 * @param reply_buf buffer for the aswer message from the relay
 *
 * @return the length of the reply message stored in reply_buf
 */
int ccnl_riot_client_get(kernel_pid_t relay_pid, char *name, char *reply_buf);

/**
 * @brief   high level function to publish a name, e.g. "/riot/test"
 *          all interest with "prefix" as prefix  received by the rely
 *          are forwarded to this thread
 *
 * @note    this function uses riot_new_face and riot_register_prefix
 *
 * @param relay_pid pid of the relay thread
 *
 * @param prefix c string representation of the name to publish
 *
 * @param faceid in case of "newMSGface" this is the pid to connect the face to
 *               in case of "newTRANSface" this is the network address to
 *               connect the face to
 *
 * @param type a c string may "newMSGface" for a RIOT message based face or
 *             "newTRANSface" for a network face
 *
 * @param reply_buf buffer for the aswer message from the relay
 *
 * @return the length of the reply message stored in reply_buf
 */
int ccnl_riot_client_publish(kernel_pid_t relay_pid, char *prefix, char *faceid,
        char *type, unsigned char *reply_buf);

/**
 * @brief  lower layer function to register a new face in the relay
 *
 * @param relay_pid pid of the relay
 *
 * @param type a c string may "newMSGface" for a RIOT message based face or
 *             "newTRANSface" for a network face
 *
 * @param faceid in case of "newMSGface" this is the pid to connect the face to
 *               in case of "newTRANSface" this is the network address to
 *               connect the face to
 *
 * @param reply_buf buffer for the aswer message from the relay
 *
 * @return the length of the reply message stored in reply_buf
 */
int ccnl_riot_client_new_face(kernel_pid_t relay_pid, char *type, char *faceid,
        unsigned char *reply_buf);

/**
 * @brief lower layer function to register a new prefix
 *                      in the relay
 *
 * @param relay_pid pid of the relay
 *
 * @param prefix c string representation of the name to publish
 *
 * @param faceid in case of "newMSGface" this is the pid to connect the face to
 *               in case of "newTRANSface" this is the network address to
 *               connect the face to
 *
 * @param reply_buf buffer for the aswer message from the relay
 *
 * @return the length of the reply message stored in reply_buf
 */
int ccnl_riot_client_register_prefix(kernel_pid_t relay_pid, char *prefix,
        char *faceid, unsigned char *reply_buf);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* CCNL_RIOT_CLIENT_H */
