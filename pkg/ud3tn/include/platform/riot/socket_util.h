// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#ifndef SOCKET_UTIL_H_INCLUDED
#define SOCKET_UTIL_H_INCLUDED

/**
 * Create a new UNIX domain socket at the given path.
 *
 * @param path The path to the socket file.
 * @return the socket file descriptor, -1 on error.
 */
int create_unix_domain_socket(const char *path);

/**
 * Wait for incoming data using poll() and indicate common errors.
 *
 * @param socket_fd The socket to be polled.
 * @param timeout The maximum number of milliseconds to wait for data.
 * @return 1 if there is data to be read, 0 if the timeout passed, -1 on error.
 */
int poll_recv_timeout(const int socket_fd, const int timeout);

#endif // SOCKET_UTIL_H_INCLUDED
